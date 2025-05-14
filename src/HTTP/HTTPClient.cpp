#include "../../inc/HTTP/HTTPClient.hpp"
#include "../../inc/Webserv/Logger.hpp"
#include <memory>
#include <string>

HTTPClient::HTTPClient(
	SharedFd clientFd, 
	SharedFd serverFd, 
	std::function<void(struct epoll_event, const SharedFd&)>  addToEpoll_cb,
	std::function<std::shared_ptr<Config> (const SharedFd&, const std::string&)> getConfig_cb,
	std::function<void(const SharedFd&)> delFromEpoll_cb
	) : STATE_(RECEIVING),
		clientSock_(clientFd),
		serverSock_(serverFd),
		response_(""),
		responseGenerator_(),
		config_(NULL),
		is_cgi_requ_(false),
		first_response_(true),
		addToEpoll_cb_(addToEpoll_cb),
		getConfig_cb_(getConfig_cb),
		delFromEpoll_cb_(delFromEpoll_cb) {
}

HTTPClient::HTTPClient(const HTTPClient&& other) :
	STATE_(std::move(other.STATE_)),
	clientSock_(std::move(other.clientSock_)),
	serverSock_(std::move(other.serverSock_)),
	responseGenerator_(std::move(other.responseGenerator_)),
	config_(std::move(other.config_)),
	is_cgi_requ_(std::move(other.is_cgi_requ_)),
	first_response_(std::move(other.first_response_)),
	addToEpoll_cb_(std::move(other.addToEpoll_cb_)),
	getConfig_cb_(std::move(other.getConfig_cb_)),
	delFromEpoll_cb_(std::move(other.delFromEpoll_cb_))
{
}

HTTPClient::~HTTPClient(void) { }

/**
 * @brief checks state and processes event. Write or Read action
 * @param event epoll_event of the current event
 */
void	HTTPClient::handle(const epoll_event &event) {

	auto fd = Epoll::getEventData(event).fd;

	if (STATE_ != DONE)
	{
		if (event.events & EPOLLERR) {
			throw ClientException("Client received EPOLLER: " + std::to_string(fd));
		} else if (fd == clientSock_.get() && event.events & EPOLLHUP) {
			throw ClientException("Client received EPOLLHUP: " + std::to_string(fd));
		}
	}
	
	switch (STATE_) {
		case RECEIVING:
			handleReceiving(fd, event.events);
			break;
		case PROCESS_CGI:
			handleCGI(fd, event.events);
			break;
		case RESPONSE:
			handleResponding(fd, event.events);
			break;
		case DONE:
			return ;
	}
}

void	HTTPClient::initCGI() {
	CGIPipes pipes;
	pipes.setCallbackFunctions(clientSock_, addToEpoll_cb_, delFromEpoll_cb_);
	pipes.addNewPipes();
	cgi_ = std::make_unique<CGI>(request_, pipes, delFromEpoll_cb_);
}

/// @brief receives input, processes input, starts cgi if required
void	HTTPClient::handleReceiving(SharedFd fd, uint32_t events) {
	if (fd != clientSock_.get() || !(events & EPOLLIN)) {
		return ;
	}
	std::string data = readFrom(fd.get());
	parser_.processData(data, this);
	if (!parser_.isDone()) {
		return ;
	}
	
	request_ = parser_.getParsedRequest();
	responseGenerator_.setConfig(config_);
	// first_response_ = true;
	if (request_.status_code == 200 && CGI::isCGI(request_)) {
		is_cgi_requ_ = true;
		initCGI();
		STATE_ = PROCESS_CGI;
		return ;
	}
	STATE_ = RESPONSE;
}

/// @brief redirects epoll event to cgi object to handle it
void	HTTPClient::handleCGI(SharedFd fd, uint32_t events) {
	cgi_->handle(fd, events);
	if (cgi_->isDone()) {
		STATE_ = RESPONSE;
		return ;
	}
}

void	HTTPClient::generateResponse() {
	if (is_cgi_requ_) {
		response_ = cgiResponse();
	} else {
		response_ = responseGenerator_.generateResponse(request_);
	}
	HTTPResponse::insertHeader("Connection", "close", response_);
}

/// @brief regenerates response and add this one to the que.
void	HTTPClient::handleResponding(SharedFd fd, uint32_t events) {
	if (fd != clientSock_.get() || !(events & EPOLLOUT)) {
		return ;
	}

	if (first_response_) {
		generateResponse();
	}
	writeToClient(fd);
}

/// @brief rewrite cgi header if necessary and return response
std::string	HTTPClient::cgiResponse(void) {
	int	status =  cgi_->getStatusCode();
	std::string	response;

	if (cgi_->timedOut() || status >= 400)
	{
		HTTPRequest	cgi_error_request;
		if (status >= 500)
			cgi_error_request.request_target = "Server Error";
		else if (status >= 400)
			cgi_error_request.request_target = "Client Error";
		cgi_error_request.status_code = status;
		response = responseGenerator_.generateResponse(cgi_error_request);
	}
	else {	
		response = cgi_->getResponse();
		Logger::getInstance().log(LOG_RESPONSE, "CGI response " + std::to_string(status));
	}
	return (response);
}


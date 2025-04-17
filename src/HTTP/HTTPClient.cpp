#include "../../inc/HTTP/HTTPClient.hpp"

HTTPClient::HTTPClient(
	SharedFd clientFd, 
	SharedFd serverFd, 
	std::function<void(struct epoll_event, const SharedFd&)>  addToEpoll_cb,
	std::function<const Config* (const SharedFd&, const std::string&)> getConfig_cb,
	std::function<void(const SharedFd&)> delFromEpoll_cb
	) : clientSock_(clientFd),
		serverSock_(serverFd),
		responseGenerator_(),
		isCgiRequ_(false),
		addToEpoll_cb_(addToEpoll_cb),
		getConfig_cb_(getConfig_cb),
		delFromEpoll_cb_(delFromEpoll_cb) {
	STATE_ = RECEIVING;
	config_ = NULL;
	response_ = "";
}

HTTPClient::HTTPClient(const HTTPClient&& other) :
	STATE_(std::move(other.STATE_)),
	clientSock_(std::move(other.clientSock_)),
	serverSock_(std::move(other.serverSock_)),
	responseGenerator_(std::move(other.responseGenerator_)),
	config_(std::move(other.config_)),
	isCgiRequ_(std::move(other.isCgiRequ_)),
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

	if (event.events & EPOLLERR) {
		throw ClientException("Client received EPOLLER: " + std::to_string(fd));
	} else if (fd == clientSock_.get() && event.events & EPOLLHUP) {
		throw ClientException("Client received EPOLLHUP: " + std::to_string(fd));
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

void	printRequest(HTTPRequest request) {
	std::cerr << "------------------\nRequest:\n";
	std::cerr << "Method: " << request.method << "\n";
	std::cerr << "Target: " << request.request_target << "\n";
	std::cerr << "Host: " << request.host[0] << "\n";
	std::cerr << "StatusCode: " << request.status_code << "\n";
	std::cerr <<  "Dir on/off: " << request.dir_list << "\n------------------\n";
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
	printRequest(request_); //TODO: REMOVE
	responseGenerator_.setConfig(config_);
	if (!CGI::isCGI(request_))
	{
		STATE_ = RESPONSE;
		return ;
	}
	isCgiRequ_  = true;
	initCGI();
	STATE_ = PROCESS_CGI;
}

/// @brief redirects epoll event to cgi object to handle it
void	HTTPClient::handleCGI(SharedFd fd, uint32_t events) {
	if (fd == clientSock_.get()) {
		return ;
	}

	cgi_->handle(fd, events);
	if (cgi_->isReady()) {// TODO: rename to isDone 
		STATE_ = RESPONSE;
		return ;
	}
	STATE_ = PROCESS_CGI;
}

/// @brief regenerates response and add this one to the que.
void	HTTPClient::handleResponding(SharedFd fd, uint32_t events) {
	static bool	send_first_msg = true ;

	if (fd != clientSock_.get() || !(events & EPOLLOUT)) {
		return ;
	}

	if (send_first_msg)
	{
		if (isCgiRequ_)
			cgiResponse();
		else
		{
			responseGenerator_.generateResponse(request_);
			message_que_.push_back(responseGenerator_.loadResponse());
		}
		writeToClient(fd, send_first_msg);
		send_first_msg = false;
	}
	else
		writeToClient(fd, send_first_msg);
	if (STATE_ == DONE)
		send_first_msg = true;
}

/// @brief checks if cgi header has to be rewritten and add response to que.
void	HTTPClient::cgiResponse(void) {
	std::cerr << "is cgi response...\n";
	if (cgi_->isTimeout())
	{
		std::cerr << "is cgi timeout...\n";
		HTTPRequest	timeout_request;
		timeout_request.request_target = "timeout";
		timeout_request.status_code = 408;
		responseGenerator_.generateResponse(timeout_request);
		message_que_.push_back(responseGenerator_.loadResponse());

	}
	else {	
		message_que_.push_back(cgi_->getResponse());
	}
	std::cerr << "-------------------\nResponse: " << message_que_.back() \
	<< "\n-----------------" << std::endl;
	// cgi_.reset(); // NEED?
}


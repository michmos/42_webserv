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
		addToEpoll_cb_(addToEpoll_cb),
		getConfig_cb_(getConfig_cb),
		delFromEpoll_cb_(delFromEpoll_cb),
		isCgiRequ_(false) {
	STATE_ = RECEIVING;
	config_ = NULL;
	response_ = "";
}

// TODO: why do we need copy constructor?
HTTPClient::HTTPClient(const HTTPClient& other) :
	STATE_(other.STATE_),
	clientSock_(other.clientSock_),
	serverSock_(other.serverSock_),
	responseGenerator_(other.responseGenerator_),
	config_(other.config_),
	addToEpoll_cb_(other.addToEpoll_cb_),
	getConfig_cb_(other.getConfig_cb_),
	delFromEpoll_cb_(other.delFromEpoll_cb_),
	isCgiRequ_(other.isCgiRequ_)
{
}

HTTPClient::~HTTPClient(void) { }

/**
 * @brief checks state and processes event. Write or Read action
 * @param event epoll_event of the current event
 */
void	HTTPClient::handle(const epoll_event &event) {

	auto eventData = Epoll::getEventData(event);
	
	switch (STATE_) {
		case RECEIVING:
			STATE_ = handleReceiving(eventData);
			break;
		case PROCESS_CGI:
			STATE_ = handleCGI(eventData.fd); // TODO: why only checking fd and not flags?
			break;
		case RESPONSE:
			STATE_ = handleResponding(eventData.fd);  // TODO: why only checking fd and not flags?
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

/// @brief receives input, processes input, starts cgi if required
/// @return e_state next client state
e_state	HTTPClient::handleReceiving(struct epollEventData& evData) {
	std::string data = readFrom(evData.fd);
	parser_.processData(data, this);
	if (!parser_.isDone())
		return RECEIVING;

	request_ = parser_.getParsedRequest();
	printRequest(request_); //TODO: REMOVE
	responseGenerator_.setConfig(config_);
	if (!CGI::isCGI(request_))
	{
		return RESPONSE;
	}
	
	// start cgi
	isCgiRequ_  = true;
	CGIPipes pipes;
	pipes.setCallbackFunctions(clientSock_, addToEpoll_cb_, delFromEpoll_cb_);
	pipes.addNewPipes();
	cgi_ = std::make_unique<CGI>(request_, pipes, delFromEpoll_cb_);
	return PROCESS_CGI;
}

/// @brief redirects epoll event to cgi object to handle it
/// @return e_state next client state
e_state	HTTPClient::handleCGI(const SharedFd &fd) {
	std::vector<std::string>	env_strings;

	cgi_->handle(fd);
	if (cgi_->isReady()) // TODO: rename to isDone
		return (RESPONSE);
	return (PROCESS_CGI);
}

/// @brief regenerates response and add this one to the que.
/// @return e_state next client state
e_state	HTTPClient::handleResponding(const SharedFd &fd) {
	static bool	send_first_msg = true ;

	if (fd != clientSock_.get())
		return RESPONSE;

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
	return (DONE);
}

/// @brief checks if cgi header has to be rewritten and add response to que.
void	HTTPClient::cgiResponse(void) {
	std::cerr << "is cgi response...\n";
	if (cgi_->isTimeout())
	{
		std::cerr << "is cgi timeout...\n";
		HTTPRequest	timeout_request;
		std::memset(&timeout_request, 0, sizeof(timeout_request));
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


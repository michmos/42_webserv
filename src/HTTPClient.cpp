#include "../inc/HTTPClient.hpp"

HTTPClient::HTTPClient(SharedFd clientFd, SharedFd serverFd, 
	std::function<void(struct epoll_event)> addToEpoll_cb,
	std::function<const Config* (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb
	) : clientSock_(clientFd), serverSock_(serverFd), getConfig_cb_(getConfig_cb), \
		responseGenerator_(this) {
	pipes_.setCallbackFunction(addToEpoll_cb);
	STATE_ = RECEIVEHEADER;
	config_ = NULL;
}

HTTPClient::~HTTPClient(void) { }

bool	HTTPClient::isDone(void) {
	if (STATE_ == DONE)
		return (true);
	return (false);
}

/**
 * @brief get right Config corresponding with servername from the HTTP request
 * @param host vector string with hostname and if set port
 */
void	HTTPClient::setServer(std::vector<std::string> host) {
	Config		conf;
	std::string hostname = host[0];
	std::string port = "";

	// DO I still need the PORT?
	if (config_ == NULL)
		return ;

	if (host.size() > 1)
		port = host[1];

	config_ = getConfig_cb_(serverSock_, hostname);
}

void	HTTPClient::writeTo(int fd) {
	size_t		bytes_write;
	std::string	response;

	if (message_que_.empty())
		return ;
	response = message_que_.front();
	message_que_.erase(message_que_.begin());
	bytes_write = write(fd, response.c_str(), response.size());
	if (bytes_write != response.size())
		throw std::runtime_error("write(): " + std::string(strerror(errno)));
	
	// check if everything can be written in once?
	STATE_ = DONE;
}

std::string	HTTPClient::readFrom(int fd) {
	char buff[READSIZE + 1] = { '\0'};
	size_t	bytes_read;

	bytes_read = read(fd, buff, READSIZE);
	if (bytes_read == -1)
		throw std::runtime_error(std::string("read(): ") + strerror(errno));
	return (std::string(buff, bytes_read));
}

/**
 * @brief checks state and processes event. Write or Read action
 * @param event epoll_event of the current event
 */
void	HTTPClient::handle(epoll_event &event) {
	std::string	data;
	HTTPRequest	request;
	static bool	is_cgi_request = false;
	
	if (STATE_ == RSV_HEADER || STATE_ == RSV_BODY) // RECEIVING
	{
		data = readFrom(event.data.fd);
		parser_.addBufferToParser(data, this, STATE_);
		if (STATE_ != PARSING)
			return ;
	}

	if (STATE_ == PARSING)
		is_cgi_request = parsing();

	if (is_cgi_request) // PROCESS CGI
	{
		switch (STATE_) {
			case START_CGI:
				cgi();
				return ;
			case SEND_TO_CGI:
				cgi_->sendDataToStdin(event.data.fd); // send data to eventfd (pipe)
				STATE_ = RSV_FROM_CGI;
				return ;
			case RSV_FROM_CGI:
				cgi_->getResponseFromCGI(event.data.fd); // read data from eventfd (pipe)
				STATE_ = CRT_RSPNS_CGI;
		}
	}

	// CREATE RESPONSE & SEND
	if (STATE_ == RESPONSE)
		responding();
	else if (STATE_ == CRT_RSPNS_CGI)
		cgiresponse();
	else if (STATE_ == RESPONSE)
		writeTo(event.data.fd);
}

/// @brief parse the HTTP request header and checks if it is a cgi target
bool	HTTPClient::parsing(void) {
	request_ = parser_.getParsedRequest();
	if (!responseGenerator_.isCGI(request_))
	{
		STATE_ = RESPONSE;
		return (false);
	}
	else
	{
		STATE_ = START_CGI;
		return (true);
	}
}

/// @brief regenerates response and add this one to the que.
void	HTTPClient::responding(void) {
	responseGenerator_.setConfig();
	responseGenerator_.generateResponse(request_);
	message_que_.push_back(responseGenerator_.loadResponse());
	STATE_ = DONE;
}

/// @brief creates a CGI class, checks the method/target, starts the cgi
void	HTTPClient::cgi(void) {
	std::vector<std::string>	env_strings;
	static std::string			body = "";

	cgi_ = std::make_unique<CGI>(request_.body, pipes_.getLastPipes());
	pipes_.addNewPipes();
	if (request_.method == "DELETE")
		request_.request_target = "data/www/cgi-bin/nph_CGI_delete.py";
	else
		request_.request_target = "data/www/cgi-bin" + request_.request_target;
	cgi_->createEnv(env_strings, request_);
	cgi_->forkCGI(request_.request_target, env_strings);
	body = request_.body;
	STATE_ = SEND_TO_CGI;
}

/// @brief checks if cgi header has to be rewritten and add response to que.
void	HTTPClient::cgiresponse(void) {
	if (!cgi_->isNPHscript(request_.request_target))
		cgi_->rewriteResonseFromCGI();
	message_que_.push_back(cgi_->getResponse());
}

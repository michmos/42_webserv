#include "../inc/HTTPClient.hpp"

HTTPClient::HTTPClient(std::function<void(int, int)> callback) : \
		responseGenerator_(this) {
	pipes_.setCallbackFunction(callback);
	STATE_ = RECEIVEHEADER;
	conf_set_ = false;
	server_ = NULL;
}

HTTPClient::~HTTPClient(void) { }

bool	HTTPClient::isDone(void) {
	if (STATE_ == DONE)
		return (true);
	return (false);
}

// HOW??
void	HTTPClient::assignServerCallback(Server server) {
	server_ = &server;
}

void	HTTPClient::setServer(std::vector<std::string> host) {
	Config		conf;
	std::string hostname = host[0];
	std::string port = "";

	if (conf_set_ == true)
		return ;

	if (host.size() > 1)
		port = host[1];

	// assignServerCallback(hostname, port);

	// NOW server
	config_ = server_->getConfig();
	conf_set_ = true;
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
void	HTTPClient::work(epoll_event &event) {
	std::string	data;
	
	if (STATE_ == RECEIVEBODY || STATE_ == RECEIVEHEADER)
		data = readFrom(event.data.fd);
	else if (STATE_ == DONE)
	{
		writeTo(event.data.fd);
		// close connection?
		return ;
	}
	else if (STATE_ == CGISEND) // send data to eventfd (pipe) 
	{
		cgi_->sendDataToStdin(event.data.fd);
		STATE_ = CGIRECEIVE;
		return ;
	}
	else if (STATE_ == CGIRECEIVE) // read data from eventfd (pipe)
	{
		cgi_->getResponseFromCGI(event.data.fd);
		STATE_ = CGIRESPONSE;
	}
	feedData(std::move(data));
}

/**
 * @brief data -> receiving, parsing, responding, cgi & cgi response (Update state)
 * @param data string with read data
 */
void HTTPClient::feedData(std::string &&data) {
	HTTPRequest	request;
	
	if (STATE_ == RECEIVEHEADER || STATE_ == RECEIVEBODY)
		receiving(std::move(data));
	if (STATE_ == PARSING)
		parsing();
	if (STATE_ == RESPONSE)
		responding();
	else if (STATE_ == STARTCGI)
		cgi();
	else if (STATE_ == CGIRESPONSE)
		cgiresponse();
}

/**
 * @brief adds the data to ther parser and checks if everything is received
 * @param data string with read data
 */
void	HTTPClient::receiving(std::string &&data) {
	parser_.addBufferToParser(data, this, STATE_);
}

/// @brief parse the HTTP request header and checks if it is a cgi target
void	HTTPClient::parsing(void) {
	request_ = parser_.getParsedRequest();
	if (!responseGenerator_.isCGI(request_))
		STATE_ = RESPONSE;
	else
		STATE_ = STARTCGI;
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
	STATE_ = CGISEND;
}

/// @brief checks if cgi header has to be rewritten and add response to que.
void	HTTPClient::cgiresponse(void) {
	if (!cgi_->isNPHscript(request_.request_target))
		cgi_->rewriteResonseFromCGI();
	message_que_.push_back(cgi_->getResponse());
}

bool	HTTPClient::isConfigSet(void) { return (conf_set_); }

Config& HTTPClient::getConfig(void) {
	return (config_);
}

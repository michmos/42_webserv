#include "HTTPClient.hpp"

HTTPClient::HTTPClient(std::function<void(int, int)> callback) : \
	responseGenerator(std::make_unique<HTTPResponseGenerator>(*this)) {
	pipes_.setCallbackFunction(callback);
	STATE = RECEIVE;
}

HTTPClient::~HTTPClient(void) {
}

bool	HTTPClient::isDone(void) {
	if (STATE == DONE)
		return (true);
	return (false);
}

void	HTTPClient::assignServer(Server server) {
	server_ = &server;
}

void	HTTPClient::work(epoll_event &event) {
	char		buffer[2048];
	size_t		bytes_read;
	
	if (STATE != CGISEND && STATE != CGIRECEIVE)
	{
		// just see if read or write event?
		// reading() / writing():
		;
		if (STATE == DONE && !message_que_.empty()) // maybe better to have a seperate write and read function
		{
			std::string	response = message_que_.front();
			message_que_.erase(message_que_.begin());
			write(event.data.fd, response.c_str(), sizeof(response));
		}
	}
	else if (STATE == CGISEND) // send data to eventfd (pipe) 
	{
		write(event.data.fd, request_.body.c_str(), sizeof(request_.body));
		close(event.data.fd);
		STATE == CGIRECEIVE;
		return
	}
	else if (STATE == CGIRECEIVE) // read data from eventfd (pipe)
	{
		bytes_read = ::recv(event.data.fd, buffer, sizeof(buffer), 0);
		if (bytes_read < 0)
			throw std::runtime_error("recv errors");
		cgi_->getResponseFromCGI();
		close(event.data.fd);
		//send back to client
		STATE = CGIRESPONSE;
	}
	std::string data(buffer);
	feedData(std::move(data));
}

void HTTPClient::feedData(std::string &&data) {
	HTTPRequest	request;
	
	if (STATE == RECEIVE)
		receiving(std::move(data));
	if (STATE == PARSING)
		parsing();
	if (STATE == RESPONSE)
		responding();
	else if (STATE == STARTCGI)
		cgi();
	else if (STATE == CGIRESPONSE)
		cgiresponse();
}

void	HTTPClient::receiving(std::string &&data) {
	parser_.addBufferToParser(data);
	if (parser_.isRequestFullyParsed())
		STATE = PARSING;
}

void	HTTPClient::parsing() {
	request_ = parser_.getParsedRequest();
	if (!responseGenerator->isCGI(request_))
		STATE = RESPONSE;
	else
		STATE = STARTCGI;
}

void	HTTPClient::responding() {
	responseGenerator->generateResponse(request_);
	message_que_.push_back(responseGenerator->loadResponse());
	STATE = DONE;
}

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
	STATE = CGISEND;
}

void	HTTPClient::cgiresponse(void) {
	if (!cgi_->isNPHscript(request_.request_target))
		cgi_->rewriteResonseFromCGI();
	message_que_.push_back(cgi_->getResponse());
}

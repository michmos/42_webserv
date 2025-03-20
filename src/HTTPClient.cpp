#include "HTTPClient.hpp"

HTTPClient::HTTPClient(std::function<void(int, int)> callback) : \
	responseGenerator(std::make_unique<HTTPResponseGenerator>(*this)) {
	pipes_.setCallbackFunction(callback);
	STATE_ = RECEIVE;
}

HTTPClient::~HTTPClient(void) {
}

bool	HTTPClient::isDone(void) {
	if (STATE_ == DONE)
		return (true);
	return (false);
}

void	HTTPClient::assignServer(Server server) {
	server_ = &server;
}

void	HTTPClient::work(epoll_event &event) {
	char		buffer[2048];
	size_t		bytes_read;
	
	if (STATE_ != CGISEND && STATE_ != CGIRECEIVE)
	{
		// just see if read or write event?
		// reading() / writing():
		;
		if (STATE_ == DONE && !message_que_.empty()) // maybe better to have a seperate write and read function
		{
			std::string	response = message_que_.front();
			message_que_.erase(message_que_.begin());
			write(event.data.fd, response.c_str(), sizeof(response));
		}
	}
	else if (STATE_ == CGISEND) // send data to eventfd (pipe) 
	{
		cgi_->sendDataToStdin(event.data.fd);
		STATE_ == CGIRECEIVE;
		return ;
	}
	else if (STATE_ == CGIRECEIVE) // read data from eventfd (pipe)
	{
		cgi_->getResponseFromCGI(event.data.fd);
		//send back to client
		STATE_ = CGIRESPONSE;
	}
	std::string data(buffer);
	feedData(std::move(data));
}

void HTTPClient::feedData(std::string &&data) {
	HTTPRequest	request;
	
	if (STATE_ == RECEIVE)
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

void	HTTPClient::receiving(std::string &&data) {
	parser_.addBufferToParser(data);
	if (parser_.isRequestFullyParsed())
		STATE_ = PARSING;
}

void	HTTPClient::parsing() {
	request_ = parser_.getParsedRequest();
	if (!responseGenerator->isCGI(request_))
		STATE_ = RESPONSE;
	else
		STATE_ = STARTCGI;
}

void	HTTPClient::responding() {
	responseGenerator->generateResponse(request_);
	message_que_.push_back(responseGenerator->loadResponse());
	STATE_ = DONE;
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
	STATE_ = CGISEND;
}

void	HTTPClient::cgiresponse(void) {
	if (!cgi_->isNPHscript(request_.request_target))
		cgi_->rewriteResonseFromCGI();
	message_que_.push_back(cgi_->getResponse());
}

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
	// just see if read or write event?
	// reading() / writing():

	if (STATE == CGISEND) // send data to eventfd (pipe)
	{
		close(event.data.fd);
		STATE == CGIRECEIVE;
		return
	}
	else if (STATE == CGIRECEIVE) // read data from eventfd (pipe)
	{
		cgi_->getResponseFromCGI();
		close(event.data.fd);
		//parse CGI header to HTTP header or/and send back to client
		STATE = CGIRESPONSE;
	}
	feedData("data");
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
	std::string	response;
	int			status_code;

	response = cgi_->getResponse();
	if (cgi_->isNPHscript(request_.request_target))
		// send all to client...
		;
	else
		// parse CGI header
		;
}

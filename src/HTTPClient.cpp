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
		parsing(request);
	if (STATE == RESPONSE)
		responding(request);
	else if (STATE == STARTCGI)
		cgi(request);
	else if (STATE == CGIRESPONSE)
		cgiresponse();
}

void	HTTPClient::receiving(std::string &&data) {
	parser_.addBufferToParser(data);
	if (parser_.isRequestFullyParsed())
		STATE = PARSING;
}

void	HTTPClient::parsing(HTTPRequest &request) {
	request = parser_.getParsedRequest();
	responseGenerator->addRawData(parser_.getRawData());
	if (!responseGenerator->isCGI(request))
		STATE = RESPONSE;
	else
		STATE = STARTCGI;
}

void	HTTPClient::responding(HTTPRequest &request) {
	responseGenerator->generateResponse(request);
	STATE = DONE;
}

void	HTTPClient::cgi(HTTPRequest &request) {
	CGI 						cgi(request.body, pipes_.getLastPipes());
	std::vector<std::string>	env_strings;
	static std::string			body = "";

	pipes_.addNewPipes();
	if (request.method == "DELETE")
		request.request_target = "data/www/cgi-bin/nph_CGI_delete.py";
	else
		request.request_target = "data/www/cgi-bin" + request.request_target;
	
	cgi.createEnv(env_strings, request);
	cgi.forkCGI(request.request_target, env_strings);
	body = request.body;
	STATE = CGISEND;
}

void	cgiresponse(void) {
	if ("nph_")
		// send all to client...
		;
	else
		// parse CGI header
		;
}

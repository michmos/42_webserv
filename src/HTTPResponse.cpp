# include "HTTPResponse.hpp"
# include "CGI.hpp"

// HTTPResponseGenerator::HTTPResponseGenerator( IResponseReadyObserver &observer ) \
// 	: readyObserver_(observer) {
// 	header_ = "";
// 	body_ = "";
// 	content_type_ = "application/octet-stream";
// 	status_code_ = 200;
// 	response_ = "";
// }

HTTPResponseGenerator::HTTPResponseGenerator( ) {
	header_ = "";
	body_ = "";
	content_type_ = "application/octet-stream";
	status_code_ = 200;
	response_ = "";
}

bool	HTTPResponseGenerator::isCGI(const HTTPRequest request) {
	if (request.invalidRequest)
		return (false);
	else if (!CGI::isCgiScript(request.request_target) && request.method != "DELETE")
		return (false);
	else
		return (true);
}

HTTPResponseGenerator::~HTTPResponseGenerator(void) { }

void	HTTPResponseGenerator::generateResponse(const HTTPRequest request) {
	std::string	filename(request.request_target);

	if (request.invalidRequest)
	{
		status_code_ = 400;
		filename = "custom_404.html"; // change
	}
	else
	{
		filename_ = resolvePath(filename);
		if (filename_.empty())
			filename_ = resolvePath("custom_404.html");
		// if (filename_.empty())
		// 	return ("There went something wrong..."); // TO DO
	}
	// readyObserver_.notifyResponseReady();
}

std::string	HTTPResponseGenerator::loadResponse(void) {
	getBody();
	getContentType();
	getHttpStatusMessages();
	createHeader();
	return (header_ + body_);
}

std::string	HTTPResponseGenerator::resolvePath(std::string endpoint) {
	// TO DO: folders from config
	std::string folders[] = {
		".",
		"data",
		"data/www",
		"data/images",
		"data/www/pages",
	};
	std::string filename("");
	struct stat statbuf;
	std::string folder_file;

	if (endpoint == "/")
		endpoint = "index.html";
	if (!endpoint.empty() && endpoint.front() != '/')
		filename += "/";
	filename += endpoint;

	for (const std::string &folder : folders)
	{
		folder_file = folder + filename;
		if (stat(folder_file.c_str(), &statbuf) == 0)
		{
			if (S_ISDIR(statbuf.st_mode))
				folder_file += "index.html";
		}
		if (access(folder_file.c_str(), F_OK) != -1)
		{
			if (access(folder_file.c_str(), R_OK) == -1)
			{
				std::cerr << "No permission" << std::endl;
				return "";
			}
			return folder_file;
		}
	}
	return ("");
}

void	HTTPResponseGenerator::getBody() {
	if (!filename_.empty())
	{
		std::ifstream 		myfile(filename_, std::ios::binary);
		std::ostringstream	buffer;

		buffer.clear();
		if (!myfile)
			throw std::runtime_error("Failed to open file: " + filename_);
		buffer << myfile.rdbuf();
		body_.assign((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
	}
	else
		body_ = "";
}

void	HTTPResponseGenerator::createHeader( void ) {
	header_ = "HTTP/1.1 " + httpStatusMessages_ + "\r\n" \
			+ "Content-Type: " + content_type_ + "\r\n" \
			+ "Content-Length: " + std::to_string(body_.size()) + "\r\n\r\n"; // what if chunked?
}

void	HTTPResponseGenerator::getContentType( void )
{
	std::string	extension;
	size_t		index(filename_.find_last_of('.'));

	if (index != std::string::npos)
	{
		extension = filename_.substr(index);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		// auto it = mimetypes_.find(extension);
		// if (it != mimetypes_.end())
		// 	content_type_ = it->second;
	}
}

/// @brief checks the status and get the right httpstatus message to add it to the default error page
void	HTTPResponseGenerator::getHttpStatusMessages( void ) {
	static const std::unordered_map<int, std::string> httpStatusMessages = {
		{100, "100 Continue"}, {101, "101 Switching Protocols"}, {102, "102 Processing"},
		{200, "200 OK"}, {201, "201 Created"}, {202, "202 Accepted"}, {204, "204 No Content"},
		{301, "301 Moved Permanently"}, {302, "302 Found"}, {304, "304 Not Modified"},
		{307, "307 Temporary Redirect"}, {308, "308 Permanent Redirect"},
		{400, "400 Bad Request"}, {401, "401 Unauthorized"}, {403, "403 Forbidden"},
		{404, "404 Not Found"}, {405, "405 Method Not Allowed"}, {408, "408 Request Timeout"},
		{409, "409 Conflict"}, {410, "410 Gone"}, {415, "415 Unsupported Media Type"}, 
		{429, "429 Too Many Requests"}, {500, "500 Internal Server Error"},
		{501, "501 Not Implemented"}, {502, "502 Bad Gateway"}, {503, "503 Service Unavailable"},
		{504, "504 Gateway Timeout"}, {505, "505 HTTP Version Not Supported"}
	};

	auto it = httpStatusMessages.find(status_code_);
	if (it != httpStatusMessages.end())
		httpStatusMessages_ = it->second;
	else
		httpStatusMessages_ = "500 Internal Server Error";
	if (status_code_ != 200 && status_code_ != 404 && !body_.empty())
	{
		size_t index = body_.find("<h1>Error 404: Not found</h1>");
		if (index != body_.size())
			body_.replace(index, 30, "Error " + httpStatusMessages_);
	}
}

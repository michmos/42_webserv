# include "HTTPResponse.hpp"
# include "CGI.hpp"

// HTTPResponseGenerator::HTTPResponseGenerator( IResponseReadyObserver &observer ) \
// 	: readyObserver_(observer) {
// 	header_ = "";
// 	body_ = "";
// 	content_type_ = "application/octet-stream";
// 	status_code_ = 200;
// 	raw_data_ = "";
// 	response_ = "";
// }

HTTPResponseGenerator::HTTPResponseGenerator( ) {
	header_ = "";
	body_ = "";
	content_type_ = "application/octet-stream";
	status_code_ = 200;
	raw_data_ = "";
	response_ = "";
}

std::vector<char*> createEnv(std::vector<std::string> &envStrings, const HTTPRequest request) {
	envStrings.push_back("REQUEST_METHOD=" + request.method);
	envStrings.push_back("REQUEST_TARGET=" + request.request_target);
	envStrings.push_back("CONTENT_LENGTH=" + std::to_string(request.body.size()));
	for (const auto& pair : request.headers)
	{
		if (*pair.second.end() == '\n')
			envStrings.push_back(pair.first + "=" + pair.second.substr(0, pair.second.size() - 1));
		else
			envStrings.push_back(pair.first + "=" + pair.second);
	}

	std::vector<char*> env;
	for (auto &str : envStrings)
		env.push_back(const_cast<char*>(str.c_str()));
	env.push_back(nullptr);
	return env;
}

HTTPResponseGenerator::~HTTPResponseGenerator() { }

std::string HTTPResponseGenerator::getResponseCGI( void ) {
	return (response_);
}

void	HTTPResponseGenerator::generateResponse( const HTTPRequest request, int epoll_fd) {
	std::string					filename(request.request_target);
	std::vector<std::string>	env_strings;

	if (request.invalidRequest)
	{
		status_code_ = 400;
		filename = "custom_404.html"; // change
	}
	std::cerr << "filename generateResponse: " << filename << std::endl;
	if (!CGI::isCgiScript(filename) && request.method != "DELETE")
	{
		filename_ = resolvePath(filename);
		if (filename_.empty())
			filename_ = resolvePath("custom_404.html");
		// if (filename_.empty())
		// 	return ("There went something wrong..."); // TO DO
	}
	else
	{
		if (request.method == "DELETE")
			filename = "data/www/cgi-bin/nph_CGI_delete.py";
		else
			filename = "data/www/cgi-bin" + filename;
		createEnv(env_strings, request);

		CGI	cgi(request.body);
		cgi.addEventWithData(epoll_fd);
		cgi.forkCGI(filename, env_strings);
		response_ = cgi.getResponse(); //???
	}
	// readyObserver_.notifyResponseReady();
}

std::string	HTTPResponseGenerator::loadResponse( void ) {
	getBody();
	getContentType();
	getHttpStatusMessages();
	createHeader();
	return (header_ + body_);
}

std::string	HTTPResponseGenerator::resolvePath( std::string endpoint ) {
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
	return "";
}

// this is more a one time thing WHERE TO PUT IT?? PARSING?
void	HTTPResponseGenerator::loadMimeTypes( const std::string& filename ) {
	size_t			delimiter;
	std::string		extension;
	std::string		mimetype;
	std::string		line;
	std::ifstream	file(filename);

	if (!file)
		throw std::runtime_error("Failed to open file: " + filename);

	while (std::getline(file, line))
	{
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty() || line[0] == '#')
			continue;

		delimiter = line.find('=');
		if (delimiter == std::string::npos && delimiter == line.length() - 1)
			continue ;

		extension = line.substr(0, delimiter);
		mimetype = line.substr(delimiter + 1);
		extension.erase(0, extension.find_first_not_of(" \t"));
		extension.erase(extension.find_last_not_of(" \t") + 1);
		mimetype.erase(0, mimetype.find_first_not_of(" \t"));
		mimetype.erase(mimetype.find_last_not_of(" \t") + 1);
		mimetypes_[extension] = mimetype;
	}
}

void	HTTPResponseGenerator::getBody() {
	if (!filename_.empty())
	{
		std::ifstream 		myfile(filename_, std::ios::binary);
		std::ostringstream	buffer;

		buffer.clear();
		if (!myfile)
			throw std::runtime_error("Failed to open file: " + filename_);
		// buffer << myfile.rdbuf();
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

		auto it = mimetypes_.find(extension);
		if (it != mimetypes_.end())
			content_type_ = it->second;
	}
}

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

void	HTTPResponseGenerator::addRawData(std::string data) {
	raw_data_ = data;
}
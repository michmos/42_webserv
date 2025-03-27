# include "../inc/HTTPResponse.hpp"

HTTPResponseGenerator::HTTPResponseGenerator(HTTPClient *client) : client_(client) {
	header_ = "";
	body_ = "";
	content_type_ = "application/octet-stream";
	status_code_ = 200;
	response_ = "";
}

HTTPResponseGenerator::~HTTPResponseGenerator(void) { }

void	HTTPResponseGenerator::setConfig(void) {
	config_ = &client_->getConfig();
}

bool	HTTPResponseGenerator::isCGI(const HTTPRequest request) {
	if (request.invalidRequest)
		return (false);
	else if (!CGI::isCgiScript(request.request_target) && request.method != "DELETE")
		return (false);
	else
		return (true);
}

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
		{
			// check if err_pages
			filename_ = resolvePath("custom_404.html");
		}
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


std::string HTTPResponseGenerator::searchThroughIndices(std::vector<std::string> indices, bool autoindex) {
	std::string	folder_file;
	
	if (indices.empty() && autoindex == false)
	{
		status_code_ = 404;
		return ("");
	}
	else if (indices.empty())
	{
		dir_list_ = true;
		return ("");
	}
	else
	{
		for (const std::string &index : indices)
		{
			folder_file = checkFolder(index);
			if (!folder_file.empty() || status_code_ == 403)
				return (folder_file);
			status_code_ = 200;
		}
		status_code_ = 404;
		return ("");
	}
}
/**
 * @brief checks if access is okay and adds indexpages if necessary
 * @param filename string with filenname
 * @return path or empty when an error appears
 */
std::string	HTTPResponseGenerator::checkFolder(std::string path) {
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) == 0)
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			if (config_->getAutoindex(path))
			{
				dir_list_ = true;
				return ("");
			}
			else
				return (searchThroughIndices(config_->getIndex(path), false));
		}
	}
	if (access(path.c_str(), F_OK) != -1)
	{
		if (access(path.c_str(), R_OK) == -1)
		{
			std::cerr << "No permission" << std::endl;
			status_code_ = 403;
			return "";
		}
		return (path);
	}
	status_code_ = 404;
	return ("");
}

/**
 * @brief path + file access, corrects index and checks autoindex on/off
 * @param endpoint string with request target
 * @return folder + endpoint to read from
 */
std::string	HTTPResponseGenerator::resolvePath(std::string endpoint) {
	dir_list_ = false;
	if (endpoint == "/")
	{
		if (config_->getAutoindex(endpoint) == false)
		{
			status_code_ = 403;
			return ("");
		}
		endpoint = searchThroughIndices(config_->getIndex(endpoint), true);
	}
	else
		endpoint = checkFolder(endpoint);
	return (endpoint);
}

/**
 * @brief sums up all the directives from the given directory
 * @param path const char * with dir name
 * @return string with all the directories, seperated with a '\n'
 */
static std::string	getAllDirNames(const char *path) {
	struct dirent 	*d;
	std::string		list;
	DIR 			*dir = opendir(path);
	
	if (!dir)
		return ("");
	while (1)
	{
		d = readdir(dir);
		if (d != NULL)
			list += d->d_name + '\n';
		else
			break ;
	}
	closedir(dir);
	return (list);
}

/// @brief get body from request target
void	HTTPResponseGenerator::getBody(void) {
	if (dir_list_ == true)
		body_ = getAllDirNames(filename_.c_str());
	else if (!filename_.empty())
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

/// @brief combines all parts of HTTP header and adds right values
void	HTTPResponseGenerator::createHeader(void) {
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
		// content_type_ = config_.getMimetype(extension) 
	}
}

/// @brief checks the status and get the right httpstatus message to add it to the default error page
void	HTTPResponseGenerator::getHttpStatusMessages(void) {
	static const std::unordered_map<int, std::string> httpStatusMessages = {
		{100, "100 Continue"}, {101, "101 Switching Protocols"}, {102, "102 Processing"},
		{200, "200 OK"}, {201, "201 Created"}, {202, "202 Accepted"}, {204, "204 No Content"},
		{301, "301 Moved Permanently"}, {302, "302 Found"}, {304, "304 Not Modified"},
		{307, "307 Temporary Redirect"}, {308, "308 Permanent Redirect"},
		{400, "400 Bad Request"}, {401, "401 Unauthorized"}, {403, "403 Forbidden"},
		{404, "404 Not Found"}, {405, "405 Method Not Allowed"}, {408, "408 Request Timeout"},
		{409, "409 Conflict"}, {410, "410 Gone"}, {413 ,"413 Payload Too Large"}, {415, "415 Unsupported Media Type"}, 
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

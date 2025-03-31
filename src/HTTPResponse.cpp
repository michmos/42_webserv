# include "../inc/HTTPResponse.hpp"

HTTPResponse::HTTPResponse(HTTPClient *client) : client_(client) {
	header_ = "";
	body_ = "";
	content_type_ = "application/octet-stream";
	status_code_ = 200;
	response_ = "";
}

HTTPResponse::~HTTPResponse(void) { }

void	HTTPResponse::setConfig(void) {
	config_ = client_->getConfig();
}

bool	HTTPResponse::isCGI(const HTTPRequest request) {
	if (request.invalidRequest)
		return (false);
	else if (!CGI::isCgiScript(request.request_target) && request.method != "DELETE")
		return (false);
	else
		return (true);
}

// void	HTTPResponse::checkErrorPages(std::string filename, std::vector<std::string> error_pages) {
// 	int	status_code = status_code_;

// 	for (std::string &error_page : config_->getRedirect(filename))
// 	{
// 		if ()
// 	}

// }

void	HTTPResponse::generateResponse(const HTTPRequest request) {
	std::string	filename(request.request_target);

	if (request.invalidRequest)
	{
		// statuscode check.
		status_code_ = 400;
		filename = "custom_404.html"; // change
	}
	else
	{
		filename_ = getEndpointPath(filename);
		if (filename_.empty())
		{
			// check if err_pages
			filename_ = getEndpointPath("custom_404.html");
		}
	}
}

std::string	HTTPResponse::loadResponse(void) {
	getBody();
	getContentType();
	getHttpStatusMessages();
	createHeader();
	return (header_ + body_);
}


std::string HTTPResponse::searchThroughIndices(std::vector<std::string> indices, bool autoindex) {
	if (indices.empty())
	{
		if (autoindex)
			dir_list_ = true;
		else
			status_code_ = 404;
		return ("");
	}

	std::string	folder_file;
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

std::string	HTTPResponse::handleDir(const std::string &path) {
	if (config_->getAutoindex(path))
	{
		dir_list_ = true;
		return ("");
	}
	else
		return (searchThroughIndices(config_->getIndex(path), false));
}

std::string	HTTPResponse::handleAccess(const std::string &path) {
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
 * @brief checks access for file or dir
 * @param filename string with filenname
 * @return path or empty when an error appears
 */
std::string	HTTPResponse::checkFolder(std::string path) {
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
		return (handleDir(path));
	return (handleAccess(path));
}

/**
 * @brief path + file access, corrects index and checks autoindex on/off
 * @param endpoint string with request target
 * @return folder + endpoint to read from
 */
std::string	HTTPResponse::getEndpointPath(std::string endpoint) {
	dir_list_ = false;

	if (endpoint == "/")
	{
		if (config_->getAutoindex(endpoint) == false)
		{
			status_code_ = 403;
			return ("");
		}
		else
			return (searchThroughIndices(config_->getIndex(endpoint), true));
	}
	return (checkFolder(endpoint));
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
			list += d->d_name + std::string("\n");
		else
			break ;
	}
	closedir(dir);
	return (list);
}

/// @brief get body from request target
void	HTTPResponse::getBody(void) {
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
void	HTTPResponse::createHeader(void) {
	header_ = "HTTP/1.1 " + httpStatusMessages_ + "\r\n" \
			+ "Content-Type: " + content_type_ + "\r\n" \
			+ "Content-Length: " + std::to_string(body_.size()) + "\r\n\r\n"; // what if chunked?
}

/// @brief search through mimetypes to set the right content type based on the extension
void	HTTPResponse::getContentType( void )
{
	std::string	extension;
	size_t		index(filename_.find_last_of('.'));

	if (index == std::string::npos)
		return ;

	extension = filename_.substr(index + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	for (const auto &type : config_->getMimeTypes())
	{
		for(const std::string &mimetype : type.second)
		{
			if (mimetype == extension)
			{
				content_type_ = type.first;
				return ;
			}
		}
	}
}

/// @brief checks the status and get the right httpstatus message to add it to the default error page
void	HTTPResponse::getHttpStatusMessages(void) {
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

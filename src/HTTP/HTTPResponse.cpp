# include "../../inc/HTTP/HTTPResponse.hpp"

// #######################     PUBLIC     ########################
// ###############################################################

HTTPResponse::HTTPResponse(void) {
	header_ = "";
	body_ = "";
	content_type_ = "application/octet-stream";
	status_code_ = 200;
	response_ = "";
	dir_list_ = false;
	config_ = NULL;
}

HTTPResponse::~HTTPResponse(void) { }

bool	HTTPResponse::isCGI(const HTTPRequest request) {
	if (request.invalidRequest)
		return (false);
	else if (!CGI::isCgiScript(request.request_target) && request.method != "DELETE")
		return (false);
	else
		return (true);
}

void	HTTPResponse::setConfig(const Config *conf) { config_ = conf; }

static bool	isRedirectStatusCode(int status_code) { return (status_code >= 300 && status_code <= 308); }

/**
 * @brief generates Response by processing the request Header 
 * @param request struct that contains all the parsed header info
 */
void	HTTPResponse::generateResponse(const HTTPRequest request) {
	std::string	filename(request.request_target);
	std::cerr << request.method << " " << request.request_target << request.status_code << std::endl;

	if (request.dir_list)
		dir_list_ = true ;
	if (!request.invalidRequest && request.status_code == 200)
		filename_ = request.request_target;
	else if (isRedirectStatusCode(request.status_code)) // redirect
		header_ = request.body;
	else if (isCustomErrorPages(filename, request.status_code))
		filename_ = searchErorPage(request.subdir, filename);
}

std::string	HTTPResponse::loadResponse(void) {
	std::cerr << "load response" << std::endl;
	if (isRedirectStatusCode(status_code_))
		return (header_);
	getBody();
	getContentType();
	getHttpStatusMessages();
	createHeader();
	return (header_ + body_);
}

// ##############     PRIVATE GENERATE RESPONSE     ##############
// ###############################################################

/**
 * @brief search for the fullpath for the custom errorpage
 * @param dir vector<string> with all the subdirectories
 * @param errorpage string with the filename of the errorpage
 * @return fullpath if found, else empty
 */
std::string	HTTPResponse::searchErorPage(const std::vector<std::string> &dir, const std::string &errorpage) {
	struct stat	statbuf;

	for (const std::string &path : dir)
	{
		std::string fullpath = path + errorpage;
		if (stat(fullpath.c_str(), &statbuf) == 0)
		{
			if (access(fullpath.c_str(), F_OK) != -1)
			{
				if (access(fullpath.c_str(), R_OK) == -1)
				{
					std::cerr << "No permission" << std::endl;
					status_code_ = 403;
					return "";
				}
				return (fullpath);
			}
			break ;
		}
	}
	status_code_ = 404;
	return ("");
}

bool	HTTPResponse::isCustomErrorPages(std::string &filename, int status_code) {
	std::string error_page = config_->getErrorPage(status_code);
	if (error_page.empty())
		return (false);
	filename = error_page;
	return (true);
}

// ################     PRIVATE LOAD RESPONSE     ################
// ###############################################################

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
	list = "<!DOCTYPE html><head><title>" + std::string(path) + \
		"Folder:</title></head><body><p>" \
		+ std::string(path) + "</p><br><ul>";
	while (true)
	{
		d = readdir(dir);
		if (d != nullptr)
			list += "<li>" + std::string(d->d_name) + "</li>";
		else
			break ;
	}
	closedir(dir);
	list += "</ul></body></html>";
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
		body_.assign((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
	}
	else
		body_ = "";
}

/// @brief search through mimetypes to set the right content type based on the extension
void	HTTPResponse::getContentType( void )
{
	std::string	extension;
	size_t		index;

	if (dir_list_)
	{
		content_type_ = "text/html";
		return ;
	}
	index = filename_.find_last_of('.');
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

/// @brief combines all parts of HTTP header and adds right values
void	HTTPResponse::createHeader(void) {
	header_ = "HTTP/1.1 " + httpStatusMessages_ + "\r\n" \
			+ "Content-Type: " + content_type_ + "\r\n" \
			+ "Content-Length: " + std::to_string(body_.size()) + "\r\n\r\n"; // what if chunked?
}

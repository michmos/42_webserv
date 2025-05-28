
# include "../../inc/HTTP/HTTPResponse.hpp"
# include "../../inc/Webserv/Logger.hpp"
#include <cstddef>
#include <algorithm>
#include <string>
#include <fstream>

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

void	HTTPResponse::setConfig(const std::shared_ptr<Config> config) { config_ = config; }

static bool	isRedirectStatusCode(int status_code) { return (status_code >= 300 && status_code <= 308); }

/**
 * @brief processes request header
 * @param request struct that contains all the parsed header info
 */
void	HTTPResponse::procsRequHeader(HTTPRequest request) {
	std::string	filename(request.request_target);

	if (request.dir_list) { 	// check directory listing 
		if (!config_->getAutoindex(filename)) {
			request.status_code = 404;
		} else {
			dir_list_ = true ;
		}
		Logger::getInstance().log(LOG_DEBUG, "is directory listing");
	}

	if (!request.invalidRequest && request.status_code == 200) {	// Normal file serving
		filename_ = request.request_target;
	}
	else if (request.status_code >= 400){  // Custom err page file serving
		std::string errPage = config_->getErrorPage(request.status_code);
		if (!errPage.empty()) {
			filename_ = searchErrorPage(config_->getRoot("/"), errPage);
		}
	}
	status_code_ = request.status_code;
}

/**
 * @brief insert "key: value\r\n" after first linebreak or first line
 */
void	HTTPResponse::insertHeader(const std::string& key, const std::string& value, std::string& response) {
	size_t	insertPos;
	size_t	firstLineBreak = response.find("\r\n");
	if (firstLineBreak != std::string::npos) {
		insertPos = firstLineBreak + 2;
	} else {
		response += "\r\n";
		insertPos = response.size();
	}

	std::string headerLine = key + ": " + value + "\r\n";
	response.insert(insertPos, headerLine);
}

std::string	HTTPResponse::generateResponse(const HTTPRequest& request) {
	procsRequHeader(request);
	setBody();
	setHeader(request);
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
std::string	HTTPResponse::searchErrorPage(const std::vector<std::string> &dir, const std::string &errorpage) {
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

// ################     PRIVATE LOAD RESPONSE     ################
// ###############################################################

/**
 * @brief creates a directory listing in html format
 * @param path const char * with dir name
 * @return string with all the directories in html syntax
 */
static std::string	createDirListingHtml(const char *path) {
	struct dirent 	*d;
	std::string		list;
	DIR 			*dir = opendir(path);

	if (!dir)
		return ("");
	list = "<!DOCTYPE html><head><title>" + std::string(path) + \
		"Folder:</title></head><body><p>" \
		+ std::string(path) + "</p><br><ul>";
	do {
		d = readdir(dir);
		if (d != nullptr)
			list += "<li>" + std::string(d->d_name) + "</li>";
	} while (d);
	list += "</ul></body></html>";

	closedir(dir);
	return (list);
}

/// @brief get body from request target
void	HTTPResponse::setBody(void) {
	if (dir_list_ == true) { // create directory listing html page
		body_ = createDirListingHtml(filename_.c_str());
	}
	else if (!filename_.empty()) // copy file content to body
	{
		std::ifstream	myfile(filename_, std::ios::binary);
		if (!myfile)
			throw std::runtime_error("Failed to open file: " + filename_);

		body_.assign((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
	}
}

/// @brief search through mimetypes to set the right content type based on the extension
void	HTTPResponse::setContentType( void )
{
	std::string	extension;

	if (dir_list_)
	{
		content_type_ = "text/html";
		return ;
	}

	// get file extension
	size_t index = filename_.find_last_of('.');
	if (index == std::string::npos)
		return ;
	extension = filename_.substr(index + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	// find mime type
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
std::string	HTTPResponse::getHttpStatusMessages(int statusCode) {
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

	auto it = httpStatusMessages.find(statusCode);
	if (it != httpStatusMessages.end())
		return(it->second);
	return("500 Internal Server Error");
}

/// @brief combines all parts of HTTP header and adds right values
void	HTTPResponse::setHeader(const HTTPRequest& request) {
	setContentType();
	httpStatusMessages_ = getHttpStatusMessages(status_code_);

	header_ = "HTTP/1.1 " + httpStatusMessages_ + "\r\n"
			+ "Content-Type: " + content_type_ + "\r\n";
	if (request.redir_ && isRedirectStatusCode(request.status_code)) { // Redirect response
		header_ += "Location: " + request.request_target;
	}
	if (!body_.empty())
		header_ += "Content-Length: " + std::to_string(body_.size());
	header_ += "\r\n\r\n";
}

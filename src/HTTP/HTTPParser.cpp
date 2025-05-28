# include "../../inc/HTTP/HTTPParser.hpp"
# include "../../inc/Webserv/Logger.hpp"
# include "../../inc/Config/Config.hpp"
# include "../../inc/HTTP/HTTPClient.hpp"
#include <cstddef>
#include <algorithm>
#include <sstream>
#include <set>
#include <unistd.h>

HTTPParser::HTTPParser(void) : \
	content_length_(0), chunked_(false) {
	result_.status_code = 200;
	result_.invalidRequest = false;
	result_.body = "";
	result_.method = "";
	result_.request_target = "";
	result_.dir_list = false;
	PARSE_STATE_ = RCV_HEADER;
}

HTTPParser::~HTTPParser(void) { }

bool	HTTPParser::isHeaderRead(void) {
	if (rawRequest_.find("\r\n\r\n") == std::string::npos)
		return (false);
	return (true);
}

/// @brief splits the header and the (first part of) the body;
void	HTTPParser::splitHeaderBody(void) {
	size_t header_end;

	header_end = rawRequest_.find("\r\n\r\n");
	header_ = rawRequest_.substr(0, header_end);
	if (header_end != rawRequest_.size())
		result_.body = rawRequest_.substr(header_end + 4, rawRequest_.size() - (header_end + 4));
}

/// @brief if available, checks if content length is same as size body
bool	HTTPParser::isBodyComplete(size_t size) {
	if (content_length_ > 0)
		return (size == content_length_);
	else
		return (true);
}

/**
 * @brief set content_length_ if possible
 * @param str string with line that contains Content-Lenght
 * @TRHOW if invalid content length input
 */
void	HTTPParser::parseContentLength(std::string str) {
	std::string	str_nr;
	size_t		cl_pos;
	size_t		start;
	size_t		end;

	cl_pos = str.find("Content-Length:");
	if (cl_pos == std::string::npos)
		throw InvalidRequestException("Invalid content length");
	str_nr = str.substr(cl_pos + 15);
	start = str_nr.find_first_not_of(" \n\r\t");
	end = str_nr.find_last_not_of(" \n\r\t");
	if (start == std::string::npos)
		throw InvalidRequestException("Invalid content length");
	try
	{
		content_length_ = std::stol(str_nr.substr(start, end - start + 1));
	}
	catch (const std::exception& e)
	{
		throw InvalidRequestException("Invalid content length");
	}
}

/**
 * @brief header needs a method, target or protocol
 * @THROW if request line is invalid
 */
void	HTTPParser::verifyRequestLine(const Config& config) {
	if (!isAllwdMeth(result_.method, config))
	{
		result_.status_code = 405;
		throw InvalidRequestException("Unallowed method in request line");
	}
	if (result_.method.empty())
		throw InvalidRequestException("Empty method in request line");
	else if (result_.request_target.empty())
		throw InvalidRequestException("Empty request target in request line");
	else if (result_.protocol.empty())
		throw InvalidRequestException("Empty protocol target in request line");
}

/**
 * @brief extracts HTTP protocol, request target and if available query string
 * @param str line with HTTP in it
 * @THROW throws exception if invalid line encountered
 */
void	HTTPParser::parseHTTPline(const std::string &str) {
	size_t	split;
	size_t	second_space;
	size_t	query_index;

	// get method
	split = str.find(" ");
	if (split == std::string::npos)
		throw InvalidRequestException("Invalid request line");
	result_.method = str.substr(0, split);

	// get request uri
	second_space = str.find(" ", split + 1);
	if (second_space == std::string::npos)
		throw InvalidRequestException("Invalid request line");
	result_.request_uri = str.substr(split + 1, second_space - split - 1);

	// extract request_target and query string from request_uri
	query_index = result_.request_uri.find("?");
	if (query_index != std::string::npos)
	{
		result_.headers["QUERY_STRING"] = result_.request_uri.substr(query_index + 1);
		result_.request_target = result_.request_uri.substr(0, query_index);
	} else {
		result_.headers["QUERY_STRING"] = "";
		result_.request_target = result_.request_uri;

	}
	result_.protocol = str.substr(second_space + 1);
}

/**
 * @brief removes spaces in begin and end from value
 * @param str raw value string
 * @return trimmed string
 */
static std::string trimString(const std::string &str) {
	size_t	start;
	size_t	end;

	if (str.empty())
		return ("");
	start = str.find_first_not_of(' ');
	end = str.find_last_not_of(' ');
	if (start == std::string::npos || start == end)
		return ("");
	return (str.substr(start, end - start + 1));
}

/**
 * @brief extract key-value from header,in right format in unordmap (CAPITAL_KEY=trimmedString)
 * @param str line from header
 */
void	HTTPParser::parseExtraHeaderInformation(const std::string &str) {
	size_t		split;
	std::string	raw_key;
	std::string key("");

	split = str.find(":");
	if (split == std::string::npos || split == str.size())
		return ;
	raw_key = str.substr(0, split);
	for (size_t i = 0; i < raw_key.size(); i++)
	{
		if (raw_key[i] == '-')
			key += '_';
		else if (std::isalpha(raw_key[i]))
			key += static_cast<char>(std::toupper(raw_key[i]));
		else
			key += raw_key[i];
	}
	result_.headers[key] = trimString(str.substr(split + 1));
}

static std::vector<std::string>	getHost(std::string &host_string) {
	std::vector<std::string>	host;
	size_t						split;
	std::string					hostname;
	std::string					port;

	size_t find = host_string.find_last_of("\r");
	if (find != std::string::npos && find == host_string.length() - 1) {
		host_string.erase(find);
	}

	if (host_string.find(":") != std::string::npos)
		host_string.erase(0, 5);

	split = host_string.find(":");
	if (split != std::string::npos)
	{
		hostname = host_string.substr(0, split);
		port = host_string.substr(split + 1);
		host.push_back(hostname);
		host.push_back(port);
	}
	else
		host.push_back(host_string);
	return (host);
}

static void split_and_trim(std::vector<std::string> &encoding_items, const std::string& input) {
	std::stringstream			ss(input);
	std::string					item;

	while (std::getline(ss, item, ','))
	{
		item = trimString(item);
		if (item.empty())
			throw InvalidRequestException("Tranfer-Encoding has a empty value");
		encoding_items.push_back(item);
	}
}

static void	validateTransferEncoding(std::string str) {
	std::vector<std::string>				encoding_items;
	static const std::set<std::string> 	valid_encodings = { \
		"chunked", "gzip", "compress", "deflate", "identity"};

	split_and_trim(encoding_items, str);
	if (encoding_items.empty())
		throw InvalidRequestException("Tranfer-Encoding value is missing");
	for (std::string item : encoding_items)
	{
		if (valid_encodings.find(item) != valid_encodings.end())
			throw InvalidRequestException("Invalid Tranfer-Encoding value");
	}
}

/**
 * @brief extract all info from header and validate
 * @THROW if invalidRequest
 */
void	HTTPParser::parseHeader(void) {
	std::istringstream 	is(header_);
	std::string			str;

	while (std::getline(is, str))
	{
		if (str.find("Host:") != std::string::npos)
			result_.host = getHost(str);
		else if (result_.method.empty() && str.find("HTTP/") != std::string::npos)
			parseHTTPline(str);
		else if (str.find("Transfer-Encoding:") != std::string::npos)
		{
			validateTransferEncoding(str.substr(19));
			chunked_ = true;
		}
		else if (str.find("Content-Length:") != std::string::npos)
			parseContentLength(str);
		else
			parseExtraHeaderInformation(str);
	}
}

/**
 * @brief saves chunks and checks when chunksize == 0 (ready);
 * @param buff std::string with readbuffer;
 */
void	HTTPParser::addIfProcessIsChunked(const std::string &buff) {
	std::string	raw_body(result_.body + buff);
	std::string	chunk_size_str;
	size_t		found;
	size_t		chunk_size;
	size_t		pos(0);

	while ((found = raw_body.find("\r\n", pos)) != std::string::npos)
	{
		chunk_size_str = raw_body.substr(pos, found - pos);
		chunk_size = std::stoi(chunk_size_str, nullptr, 16);
		if (chunk_size == 0)
		{
			PARSE_STATE_ = DONE_PARSING;
			return ;
		}
		pos = found + 2;
		if (pos + chunk_size <= raw_body.size())
		{
			result_.body += raw_body.substr(pos, chunk_size);
			pos += chunk_size + 2;
		}
	}
}

bool	HTTPParser::isAllwdMeth(const std::string& method, const Config& conf) {
	std::vector<std::string>	allwdMeth;

	allwdMeth = conf.getMethods(result_.request_target);
	return (std::find(allwdMeth.begin(), allwdMeth.end(), method) != allwdMeth.end());
}

static std::string	addDir_Folder(std::string root, std::string path, std::string folder) {
	std::string	full_path = "";

	if (!root.empty())
	{
		if (root.back() == '/')
			root.pop_back();
		full_path = root;
	}
	if (path.back() == '/')
		path.pop_back();
	if (folder.front() != '/')
		full_path += path + '/' + folder;
	else
		full_path += path + folder;
	return (full_path);
}

static bool	hasRightPermission(const std::string &path, int &status_code) {
	if (access(path.c_str(), F_OK) != -1)
	{
		if (access(path.c_str(), R_OK) == -1)
		{
			status_code = 403;
			return (false);
		}
		status_code = 200;
		return (true);
	}
	status_code = 404;
	return (false);
}

bool	isAccessible(std::string &fullpath, bool &dir_list, int &statuscode, bool autoindex) {
	struct stat	statbuf;
	
	if (stat((fullpath).c_str(), &statbuf) == 0)
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			if (fullpath.back() == '/' && autoindex)
			{
				dir_list = true;
				return (true);
			}
			else if (fullpath.back() == '/')
				statuscode = 403;
			return (false);
		}
		else if (hasRightPermission(fullpath, statuscode))
			return (true);
	}
	return (false);
}

std::string	HTTPParser::handleRootDir(const Config *config) {
	const std::vector<std::string> indices = config->getIndex("/");
	const std::vector<std::string> roots = config->getRoot("/");
	std::string path;

	for (const std::string &root : roots)
	{
		for (const std::string &index : indices)
		{
			path = root + index;
			bool autoindex = config->getAutoindex(path);
			if (isAccessible(path, result_.dir_list, result_.status_code, autoindex))
				return (path);
		}
	}

	if (config->getAutoindex("/"))
	{
		result_.dir_list = true;
		return (roots[0]);
	}
	result_.status_code = 403;
	return (result_.request_target);
}

/**
 * @brief checks for paths (redir, root, access)
 * @param config pointer to Config
 */
std::string	HTTPParser::generatePath(const Config *config) {
	const std::unordered_map<std::string, Location> loc = config->getLocations();
	std::string	full_path;
	std::string	subloc("");

	if (result_.method == "DELETE")
		return (result_.request_target);

	auto redir = config->getRedirect(result_.request_target);
	if (!redir.empty()) {
		result_.status_code = std::stoi(redir[0]);
		result_.redir_ = true;
		PARSE_STATE_ = DONE_PARSING;
		return (redir[1]);
	}
	
	if (result_.request_target == "/") {
		return (handleRootDir(config));
	}

	for (const std::string &root : config->getRoot(result_.request_target))
	{
		full_path = addDir_Folder(root, "", result_.request_target);
		bool autoindex = config->getAutoindex(full_path);
		if (isAccessible(full_path, result_.dir_list, result_.status_code, autoindex)) {
			return (full_path);
		}
	}
	if (result_.status_code == 403)
		return ("");
	result_.status_code = 404;
	return ("");
}

static bool	isBiggerMaxBodyLength(size_t content_length, uint64_t max_size) {
	return (content_length > max_size);
}

bool	HTTPParser::checkBodySizeLimit(size_t body_size, const Config *config, std::string path) {
	size_t			length;
	std::uint64_t	max_content_length = config->getClientBodySize(path);

	if (max_content_length != 0)
		length = max_content_length;
	else
		length = body_size;
	if (isBiggerMaxBodyLength(length,  max_content_length))
	{
		result_.status_code = 413;
		return (true);
	}
	if (body_size > max_content_length)
		return (true);
	else
		return (false);
}


/**
 * @brief process readbuffer by state;
 * @param buff std::string with readbuffer;
 */
void	HTTPParser::processData(std::string &buff, HTTPClient *client) {
	if (buff.empty())
		return ;
	if (result_.status_code == 413) // too big
	{
		if (buff.size() > 4 && buff.find("\r\n\r\n", buff.size() - 4) != std::string::npos)
			PARSE_STATE_ = DONE_PARSING;
	}

	switch (PARSE_STATE_) {
		case RCV_HEADER:
			rawRequest_ += buff;
			buff.clear();
			if (!isHeaderRead())
				return ;

			if (auto lb = rawRequest_.find("\n"); lb != std::string::npos) {
				Logger::getInstance().log(LOG_REQUEST, rawRequest_.substr(0, lb));
			}

			splitHeaderBody();
			try
			{
				parseHeader();
				client->setConfig(result_.host);
				result_.request_target = generatePath(client->getConfig().get());
				verifyRequestLine(*client->getConfig());
				[[fallthrough]];
			}
			catch (InvalidRequestException& e)
			{
				result_.invalidRequest = true;
				PARSE_STATE_ = DONE_PARSING;
				return;
			}
		case RCV_BODY:
			PARSE_STATE_ = RCV_BODY;
			if (chunked_)
				addIfProcessIsChunked(buff);
			else
				result_.body += buff;

			if (checkBodySizeLimit(result_.body.size(), client->getConfig().get(), result_.request_target))
				result_.status_code = 413;
			if (!isBodyComplete(result_.body.size()))
				return;
			[[fallthrough]];
		default:
			PARSE_STATE_ = DONE_PARSING;
			break;
	}
}

void	HTTPParser::clearParser(void) {
	header_.clear();
	rawRequest_.clear();
	content_length_ = 0;
	chunked_ = false;
}

const HTTPRequest HTTPParser::getParsedRequest(void) { return (result_); }

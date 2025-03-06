# include <iostream>
# include <unordered_map>
# include <sstream>


/** This was my part last time that didn't change so if it fits and we want to use it, than it is here */

struct HTTPRequest
{
	std::string method;
	std::string request_target;
	std::string protocol;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
	bool invalidRequest;
};

class HTTPParser
{
	private:
		std::string	m_rawRequest;
		std::string	m_header;
		size_t		m_content_length;
		bool		m_chunked;
		HTTPRequest	m_result;

		bool	parseContentLength( std::string str );
		void	parseExtraHeaderInformation( const std::string &str );
		bool	parseHTTPline( const std::string &str );
		bool	parseRequest( void );
		bool	isValidHeader( void );

	public:
		HTTPParser( void );
		~HTTPParser( void );
};


HTTPParser::HTTPParser( void ) : m_content_length(0) {}
HTTPParser::~HTTPParser( void ) {}

/**
 * @brief Extract the Content-Lenght and saves it as an integer
 * @param str string with Content-Length in it
 * @return true if length is not set
 */
bool	HTTPParser::parseContentLength( std::string str ) {
	size_t	split;
	
	split = str.find("Content-Length:");
	if (split == str.size())
		return (true);
	std::string strNr = str.substr(16);
	size_t start = strNr.find_first_not_of(" \n\r\t");
	size_t end = strNr.find_last_not_of(" \n\r\t");
	if (start >= end)
		return (true);
	try
	{
		if (end - start - 1 <= 0)
			return (false);
		int	nr = std::stol(strNr.substr(start, end - start + 1));
		m_content_length = nr ;
	}
	catch(...)
	{
	}
	return (false);
}

// @brief Checks if method || request_target || protocol are set, one of them is mandatory
bool	HTTPParser::isValidHeader( void ) {
	if (m_result.method.empty() || \
		m_result.request_target.empty() || \
		m_result.protocol.empty())
		return (true);
	return (false);
}

/**
 * @brief Extract info from the HTTP/1.1 target line
 * @param str HTTP/1.1 target line
 * @return bool for invalidRequest
 */
bool	HTTPParser::parseHTTPline(const std::string &str) {
	size_t	split;

	split = str.find(" ");
	if (split == std::string::npos)
		return (true);
	m_result.method = str.substr(0, split);
	size_t secondSpace = str.find(" ", split + 1);
	if (secondSpace == std::string::npos)
		return (true);
	m_result.request_target = str.substr(split + 1, secondSpace - split - 1);
	size_t query_index = m_result.request_target.find("?");
	if (query_index != std::string::npos)
	{
		m_result.headers["QUERY_STRING"] = m_result.request_target.substr(query_index + 1);
		m_result.request_target = m_result.request_target.substr(0, query_index);
	}
	else
		m_result.headers["QUERY_STRING"] = "";
	m_result.protocol = str.substr(secondSpace + 1);
	return (false);
}

static std::string remove_whitespace(std::string str) {
	if (str.empty()) 
		return "";
	size_t start = str.find_first_not_of(' ');
	size_t end = str.find_last_not_of(' ');
	if (start == std::string::npos)
		return ("");
	return (str.substr(start, end - start + 1));
}

void	HTTPParser::parseExtraHeaderInformation(const std::string &str) {
	size_t	split;

	split = str.find(":");
	if (split == std::string::npos || split == str.size())
		return ;
	std::string key = str.substr(0, split);
	std::string upper_key = "";
	for (char c: key)
		upper_key += static_cast<char>(std::toupper(c));
	for (size_t i = 0; i < upper_key.size(); i++)
	{
		if (upper_key[i] == '-')
			upper_key[i] = '_';
	}
	m_result.headers[upper_key] = remove_whitespace(str.substr(split + 1));
}

/**
 * @brief extract all info from header and validate
 * @return bool for invalidRequest
 */
bool	HTTPParser::parseRequest(void) {
	std::istringstream 	is(m_header);
	std::string			str;

	while (std::getline(is, str))
	{	
		if (m_result.method.empty() && str.find("HTTP/") != std::string::npos)
		{
			if (parseHTTPline(str))
				return (true);
		}
		else if (str.find("Transfer-Encoding:") != std::string::npos && \
				str.find("chunked") != std::string::npos)
			m_chunked = true;
		else if (str.find("Content-Length:") != std::string::npos)
		{
			if (parseContentLength(str))
				return (true);
		}
		else
			parseExtraHeaderInformation(str);
	}
	return (isValidHeader());	
}
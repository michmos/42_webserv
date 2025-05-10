#pragma once

# include <string>
# include <unordered_map>
# include <regex>
# include <vector>
# include <string>
# include <iostream>
# include <sstream>
# include <functional>
# include <filesystem>

# include <sys/stat.h>

# include "HTTPRequest.hpp"

enum e_parse_state {
	RCV_HEADER,
	RCV_BODY,
	DONE_PARSING
};

class HTTPClient;
class Config;

class HTTPParser
{
	public:
		HTTPParser( void );
		~HTTPParser( void );

		void				clearParser( void );
		void				processData( std::string &buff, HTTPClient *client );
		const HTTPRequest	getParsedRequest( void );

		inline bool isDone() {
			return (PARSE_STATE_ == DONE_PARSING);
		}

	private:
		HTTPRequest		result_;
		std::string		rawRequest_;
		std::string		header_;
		size_t			content_length_;
		bool			chunked_;
		e_parse_state	PARSE_STATE_;
		
		bool	isHeaderRead( void );
		bool	isBodyComplete( size_t size );

		void	splitHeaderBody( void );
		void	addIfProcessIsChunked( const std::string &buff );
		bool	isAllwdMeth( const std::string& method, const Config& conf );
		bool	checkBodySizeLimit( size_t body_size, const Config *config, std::string path );
		bool	isRedirection(const std::vector<std::string> &redir);
		std::string	handleRootDir( const Config *config );
		std::string	generatePath(const Config *config);

		// Parsing header
		void	parseContentLength( std::string str );
		void	parseExtraHeaderInformation( const std::string &str );
		void	parseHTTPline( const std::string &str );
		void	parseRequest( void );
		void	verifyRequestLine(const Config& config);
};

class InvalidRequestException : public std::exception {
public:
	explicit InvalidRequestException(const std::string& message) : message_(message) {}
	const char* what() const noexcept override { return message_.c_str(); }

private:
	std::string	message_;
};

# include "HTTPClient.hpp"

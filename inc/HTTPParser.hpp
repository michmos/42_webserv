#pragma once

# include <string>
# include <unordered_map>
# include <regex>
# include <vector>
# include <string>
# include <iostream>
# include <sstream>
# include <functional>

# include <sys/stat.h>

# include "HTTPRequest.hpp"
# include "HTTPClient.hpp"

typedef enum PARSE_STATE {
	RCV_HEADER,
	RCV_BODY,
	DONE
};

class HTTPParser
{
	public:
		HTTPParser( void );
		~HTTPParser( void );

		void				clearParser( void );
		void				addBufferToParser( std::string &buff, HTTPClient *client, e_state &STATE_ );
		std::string 		getRawData( void );
		const HTTPRequest	getParsedRequest( void );
		bool				isValidHeader( HTTPClient *client );

	private:
		HTTPRequest	result_;
		std::string	rawRequest_;
		std::string	header_;
		size_t		content_length_;
		bool		chunked_;
		PARSE_STATE	PARSE_STATE_;
		
		bool	isHeadersParsed( void );
		bool	verifyBodyCompletion( void );

		void	splitHeaderBody( void );
		void	addIfProcessIsChunked( const std::string &buff, e_state &STATE );
		bool	validWithConfig( HTTPClient *client ) ;

		// Parsing header
		bool	tryParseContentLength( std::string str );
		void	parseExtraHeaderInformation( const std::string &str );
		bool	parseHTTPline( const std::string &str );
		bool	parseRequest( void );
};

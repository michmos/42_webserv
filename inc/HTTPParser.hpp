#pragma once

# include <string>
# include <unordered_map>
# include <regex>
# include <vector>
# include <string>
# include <iostream>
# include <sstream>
# include <functional>

# include "HTTPRequest.hpp"
# include "HTTPClient.hpp"

typedef void (HTTPClient::*Callback)(std::vector<std::string>);

class HTTPParser
{
	public:
		HTTPParser( void );
		~HTTPParser( void );

		void				clearParser( void );

		void				addBufferToParser( std::string &buff, HTTPClient *client );
		bool				isRequestFullyParsed( void );
		std::string 		getRawData( void );
		const HTTPRequest	getParsedRequest( void );

	private:
		enum State {
			COLLECT_HEADER,
			COLLECT_BODY,
			DONE
		};

		State		current_state_;
		HTTPRequest	result_;
		std::string	rawRequest_;
		std::string	header_;
		size_t		content_length_;
		bool		chunked_;

		bool	isValidHeader( void );
		bool	isHeadersParsed( void );
		void	verifyBodyCompletion( void );

		void	splitHeaderBody( void );
		void	addIfProcessIsChunked( const std::string &buff );
		bool	compareWithConfig( HTTPClient *client ) ;

		// Parsing header
		bool	tryParseContentLength( std::string str );
		void	parseExtraHeaderInformation( const std::string &str );
		bool	parseHTTPline( const std::string &str );
		bool	parseRequest( void );
};

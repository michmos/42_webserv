#pragma once

# include <iostream>
# include <fstream>
# include <sstream>
# include <filesystem>
# include <unordered_map>
# include <algorithm>

# include <sys/stat.h>
# include <unistd.h>

# include "CGI.hpp"
# include "HTTPRequest.hpp"
# include "CGI.hpp"

class HTTPResponseGenerator {
	private:
		std::string	resolvePath( std::string endpoint );

		void	getBody( void );
		void	createHeader( void );
		void	getContentType( void );
		void	getHttpStatusMessages( void );

		std::vector<char *> env_;
		std::string			filename_;
		std::string			header_;
		std::string			body_;
		std::string			content_type_;
		std::string			httpStatusMessages_;
		std::string			response_;
		int					status_code_;

	public:
		HTTPResponseGenerator( void );
		~HTTPResponseGenerator( void );

		void		generateResponse( const HTTPRequest request );
		std::string loadResponse( void );
		bool		isCGI(const HTTPRequest request);
};
#pragma once

# include <iostream>
# include <fstream>
#include <memory>
# include <sstream>
# include <filesystem>
# include <unordered_map>
# include <algorithm>
# include <dirent.h>
# include <sys/types.h>
# include <cstring>

# include <sys/stat.h>
# include <unistd.h>

# include "../Config/Config.hpp"
# include "../CGI/CGI.hpp"

# include "HTTPRequest.hpp"

class HTTPClient;
class Config;

class HTTPResponse {
	public:
		HTTPResponse( void );
		~HTTPResponse( void );

		void		generateResponse( const HTTPRequest request );
		std::string loadResponse( void );
		void		setConfig(const std::shared_ptr<Config> config) ;

	private:
		// Generate Response
		bool		isCustomErrorPages( std::string &filename, int status_code );
		std::string	searchErrorPage( const std::vector<std::string> &dir, const std::string &errorpage );

		
		// Load Response
		void		getBody( void );
		void		getContentType( void );
		void		getHttpStatusMessages( void );
		void		createHeader( void );

		std::vector<char *> 	env_;
		std::string				filename_;
		std::string				header_;
		std::string				body_;
		std::string				content_type_;
		std::string				httpStatusMessages_;
		std::string				response_;
		int						status_code_;
		bool					dir_list_;
		std::shared_ptr<Config>	config_;
};

# include "HTTPClient.hpp"

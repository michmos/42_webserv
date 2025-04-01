#pragma once

# include <iostream>
# include <fstream>
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
		HTTPResponse( HTTPClient *client );
		~HTTPResponse( void );

		void		generateResponse( const HTTPRequest request );
		std::string loadResponse( void );
		bool		isCGI(const HTTPRequest request);
		void		setConfig(void);

	private:
		// Generate Response
		std::string	getEndpointPath( std::string endpoint );
		std::string	searchThroughIndices( std::vector<std::string> indices, bool autoindex );
		std::string	verifyFileOrDirAccess( std::string filename );
		std::string	handleAccess( const std::string &path );
		std::string	handleDir( const std::string &path );
		bool		checkErrorPages( std::string &filename, int status_code );
		
		// Load Response
		void		getBody( void );
		void		getContentType( void );
		void		getHttpStatusMessages( void );
		void		createHeader( void );

		std::vector<char *> env_;
		std::string			filename_;
		std::string			header_;
		std::string			body_;
		std::string			content_type_;
		std::string			httpStatusMessages_;
		std::string			response_;
		int					status_code_;
		bool				dir_list_;
		HTTPClient			*client_;
		const Config		*config_;
};

# include "HTTPClient.hpp"
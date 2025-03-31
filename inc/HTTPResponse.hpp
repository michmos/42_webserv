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

# include "CGI.hpp"
# include "HTTPRequest.hpp"
# include "Config.hpp"
# include "HTTPClient.hpp"

class HTTPResponse {
	public:
		HTTPResponse( HTTPClient *client );
		~HTTPResponse( void );

		void		generateResponse( const HTTPRequest request );
		std::string loadResponse( void );
		bool		isCGI(const HTTPRequest request);
		void		setConfig(void);

	private:
		

		void		getBody( void );
		void		createHeader( void );
		void		getContentType( void );
		void		getHttpStatusMessages( void );

		std::string	checkFolder( std::string filename );
		std::string	searchThroughIndices( std::vector<std::string> indices, bool autoindex );
		std::string	handleAccess( const std::string &path );
		std::string	handleDir( const std::string &path );
		std::string	getEndpointPath( std::string endpoint );

		void		checkErrorPages(std::string filename, std::vector<std::string> error_pages);

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
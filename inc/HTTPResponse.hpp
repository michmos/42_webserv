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

class HTTPResponseGenerator {
	public:
		HTTPResponseGenerator( HTTPClient *client );
		~HTTPResponseGenerator( void );

		void		generateResponse( const HTTPRequest request );
		std::string loadResponse( void );
		bool		isCGI(const HTTPRequest request);
		void		setConfig(void);

	private:
		std::string	resolvePath( std::string endpoint );

		void		getBody( void );
		void		createHeader( void );
		void		getContentType( void );
		void		getHttpStatusMessages( void );
		std::string	checkFolder( std::string filename );
		std::string	searchThroughIndices( std::vector<std::string> indices, bool autoindex );

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
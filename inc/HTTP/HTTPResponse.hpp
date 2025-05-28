#pragma once

#include <memory>
# include <dirent.h>
# include <sys/types.h>
# include <cstring>

# include <sys/stat.h>
# include <unistd.h>

# include "../Config/Config.hpp"

# include "HTTPRequest.hpp"

class HTTPClient;
class Config;

class HTTPResponse {
	public:
		HTTPResponse( void );
		~HTTPResponse( void );

		void		procsRequHeader( const HTTPRequest request );
		std::string generateResponse( const HTTPRequest& request );
		void		setConfig( std::shared_ptr<Config> config ) ;
		static void	insertHeader(const std::string& key, const std::string& value, std::string& rspns);
		static std::string	getHttpStatusMessages(int statusCode);

	private:
		// Generate Response
		bool		isCustomErrorPages( std::string &filename, int status_code );
		std::string	searchErrorPage( const std::vector<std::string> &dir, const std::string &errorpage );

		
		// Load Response
		void		setBody( void );
		void		setContentType( void );
		void		setHeader( const HTTPRequest& request );

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


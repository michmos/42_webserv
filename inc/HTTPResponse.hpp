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

class HTTPResponseGenerator {
	private:
		std::string	resolvePath( std::string endpoint );

		void	getBody( void );
		void	createHeader( void );
		void	getContentType( void );
		void	getHttpStatusMessages( void );

		std::unordered_map<std::string, std::string> mimetypes_; // change! -> in config
		std::vector<char *> env_;
		std::string			filename_;
		std::string			header_;
		std::string			body_;
		std::string			content_type_;
		std::string			httpStatusMessages_;
		std::string			response_;
		int					status_code_;
		std::string			raw_data_;
		// IResponseReadyObserver &readyObserver_; // change!
	public:
		HTTPResponseGenerator(  );
		// HTTPResponseGenerator( IResponseReadyObserver &observer ); // change!
		~HTTPResponseGenerator( void );

		void	generateResponse( const HTTPRequest request, int epoll_fd );

		void		loadMimeTypes( const std::string& filename );
		std::string loadResponse( void );
		void		addRawData( std::string data );
		std::string	getResponseCGI( void );
};
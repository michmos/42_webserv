#pragma once

# include <iostream>
# include <bitset>
# include <climits>
# include <cstring>
# include <cstdio>
# include <fstream>
# include <sstream>
# include <string>
# include <unordered_map>
# include <map>
# include <vector>
# include <cstdlib>
# include <regex>
# include <memory>
# include <any>

# include <errno.h>
# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <sys/epoll.h>

# include "HTTPRequest.hpp"
# include "HTTPRequestConfig.hpp"
# include "HTTPResponse.hpp"
# include "HTTPParser.hpp"
# include "CGI.hpp"
# include "Epoll.hpp"
# include "Server.hpp"
# include "SharedFd.hpp"
# include "CGIPipes.hpp"
# include "Config.hpp"

# define READSIZE 100

enum e_state {
	RECEIVEHEADER,
	RECEIVEBODY,
	PARSING,
	STARTCGI,
	CGISEND,
	CGIRECEIVE,
	CGIRESPONSE,
	RESPONSE,
	DONE
};

enum e_get_conf {
	AUTOINDEX,
	BODYSIZE,
	REDIRECT,
	ROOT,
	METHODS,
	INDEX
};

class HTTPClient {
	public:
		explicit HTTPClient( std::function<void(int, int)> callback );
		~HTTPClient( void );

		void	work( epoll_event &event );
		bool	isDone( void );
		void	assignServerCallback( Server server );
		void	setServer(std::vector<std::string> host);

		void		writeTo( int fd );
		std::string	readFrom( int fd );

		void	feedData( std::string &&data );
		void	receiving( std::string &&data );
		void	parsing( void );
		void	cgi( void );
		void	responding( void );
		void	cgiresponse( void );
		bool	isConfigSet(void);
	
		Config	&getConfig(void);

	private:
		e_state						STATE_;

		Server						*server_;
		std::vector<std::string>	message_que_;
		HTTPParser					parser_;
		HTTPRequest					request_;
		std::unique_ptr<CGI> 		cgi_;
		CGIPipes					pipes_;
		HTTPResponseGenerator		responseGenerator_;
		Config						config_;
		bool						conf_set_;
};

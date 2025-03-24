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

enum e_state {
	RECEIVE,
	PARSING,
	STARTCGI,
	CGISEND,
	CGIRECEIVE,
	CGIRESPONSE,
	RESPONSE,
	DONE
};

class HTTPClient {
	public:
		explicit HTTPClient( std::function<void(int, int)> callback );
		~HTTPClient( void );

		void	work( epoll_event &event );
		bool	isDone( void );
		void	assignServer( Server server );

		void	feedData( std::string &&data );
		void	receiving( std::string &&data );
		void	parsing( void );
		void	cgi( void );
		void	responding( void );
		void	cgiresponse( void );

	private:
		e_state									STATE_;

		SharedFd								fd_;
		Server									*server_;
		Epoll									ep;
		std::vector<std::string>				message_que_;
	
		HTTPParser								parser_;
		HTTPRequest								request_;
		std::unique_ptr<CGI> 					cgi_;
		CGIPipes								pipes_;
		std::unique_ptr<HTTPResponseGenerator>	responseGenerator;
};

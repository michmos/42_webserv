#pragma once

# include <poll.h>
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <string.h>
# include <bitset>
# include <climits>
# include <cstring>
# include <sys/epoll.h>
# include <errno.h>
# include <unistd.h>
# include <fcntl.h>
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

# define FROM_CHILD_READ 0
# define FROM_CHILD_WRITE 1
# define TO_CHILD_READ 2
# define TO_CHILD_WRITE 3

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


		HTTPClient( std::function<void(int, int)> callback );
		~HTTPClient( void );

		void	work( epoll_event &event );
		bool	isDone( void );
		void	assignServer( Server server );

		void	feedData( std::string &&data );
		void	receiving( std::string &&data );
		void	parsing( HTTPRequest &request );
		void	cgi( HTTPRequest &request );
		void	responding( HTTPRequest &request );
		void	cgiresponse( void );

	private:
		e_state									STATE;
		HTTPParser								parser_;
		std::unique_ptr<HTTPResponseGenerator>	responseGenerator;

		std::vector<std::vector<int>>			pipes_;
		SharedFd								fd_;
		Server									*server_;
		Epoll									ep;
		CGIPipes								pipes_;
};

#pragma once

#include <exception>
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

# include "../Config/Config.hpp"
# include "../Webserv/Epoll.hpp"
# include "../Webserv/SharedFd.hpp"
# include "../CGI/CGIPipes.hpp"
# include "../CGI/CGI.hpp"
# include "HTTPResponse.hpp"
# include "HTTPRequest.hpp"
# include "HTTPParser.hpp"

# define READSIZE 1000
# define WRITESIZE 1000
# define READY true
# define CHUNKSIZE 500

enum e_state {
	RECEIVING,
	PROCESS_CGI,
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
		HTTPClient(
			SharedFd clientFd,
			SharedFd serverFd,
			std::function<void(struct epoll_event, const SharedFd&)> addToEpoll_cb,
			std::function<const Config* (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb,
			std::function<void(const SharedFd&)> delFromEpoll_cd
		);
		HTTPClient(const HTTPClient &other);
		~HTTPClient( void );

		void	handle( const epoll_event &event );

		//ClientReadWrite
		ssize_t		writeToFd( const SharedFd &fd, const std::string &response );
		void		writeToClient( const SharedFd &fd, bool send_first_msg);
		std::string	getChunk( bool first_msg );
		std::string	getHeaderInclChunked( void );
		std::string	readFrom( int fd );

		void			cgiResponse( void );
		inline const Config	*getConfig( void ) const { return (config_); }
		inline void			setConfig(std::vector<std::string> host) { config_ = getConfig_cb_(serverSock_, host[0]); }
		inline bool			isDone( void ) const { return (STATE_ == DONE); }
	private:
		e_state						STATE_;

		SharedFd					clientSock_;
		SharedFd					serverSock_;
		std::vector<std::string>	message_que_;
		HTTPParser					parser_;
		HTTPRequest					request_;
		std::unique_ptr<CGI> 		cgi_;
		HTTPResponse				responseGenerator_;
		const Config				*config_;
		std::string					response_;
		bool						isCgiRequ_;

		std::function<void(struct epoll_event, const SharedFd&)> addToEpoll_cb_;
		std::function<const Config* (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb_;
		std::function<void(const SharedFd&)> delFromEpoll_cb_;

		e_state	handleResponding(const SharedFd &fd);
		e_state	handleCGI( const SharedFd &fd );
		e_state	handleReceiving(struct epollEventData& ev);

};

///@brief Exception specific to a client - not critical for server
class ClientException : public std::exception {
public:
	explicit ClientException(const std::string& msg) : message_(msg) {}
	const char* what() const noexcept override  { return message_.c_str(); }

private:
	std::string	message_;
};


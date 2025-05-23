#pragma once

#include <cstdint>
#include <exception>
# include <climits>
# include <cstring>
# include <cstdio>
# include <string>
# include <vector>
# include <cstdlib>
# include <memory>

# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/epoll.h>

# include "../Config/Config.hpp"
# include "../Webserv/SharedFd.hpp"
# include "../CGI/CGI.hpp"
# include "HTTPResponse.hpp"
# include "HTTPRequest.hpp"
# include "HTTPParser.hpp"

# define READSIZE 100
# define WRITESIZE 100
# define READY true

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
			std::function<std::shared_ptr<Config> (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb,
			std::function<void(const SharedFd&)> delFromEpoll_cd
		);
		HTTPClient(const HTTPClient &&other);
		~HTTPClient( void );

		void	handle( const epoll_event &event );

		//ClientReadWrite
		void		writeToFd( const SharedFd &fd, const std::string &response );
		void		writeToClient( const SharedFd &fd );
		std::string	extractChunk( void );
		std::string	getHeaderInclTransferEncoding( void );
		std::string	readFrom( int fd );

		std::string			cgiResponse( void );
		inline void			setConfig(std::vector<std::string> host) { config_ = getConfig_cb_(serverSock_, host[0]); }
		inline bool			isDone( void ) const { return (STATE_ == DONE); }
		inline const std::shared_ptr<Config>	getConfig( void ) const { return (config_); }
	private:
		e_state						STATE_;

		SharedFd						clientSock_;
		SharedFd						serverSock_;
		std::string						response_;
		HTTPParser						parser_;
		HTTPRequest						request_;
		std::unique_ptr<CGI> 			cgi_;
		HTTPResponse					responseGenerator_;
		std::shared_ptr<Config>			config_;
		bool							is_cgi_requ_;
		bool							first_response_;

		std::function<void(struct epoll_event, const SharedFd&)> addToEpoll_cb_;
		std::function<std::shared_ptr<Config> (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb_;
		std::function<void(const SharedFd&)> delFromEpoll_cb_;

		void	handleResponding(SharedFd fd, uint32_t events);
		void	generateResponse();
		void	handleCGI( SharedFd fd, uint32_t events );
		void	initCGI();
		void	handleReceiving(SharedFd fd, uint32_t events);
};

///@brief Exception specific to a client - not critical for server
class ClientException : public std::exception {
public:
	explicit ClientException(const std::string& msg) : message_(msg) {}
	const char* what() const noexcept override  { return message_.c_str(); }

private:
	std::string	message_;
};


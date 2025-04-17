#pragma once

# include <iostream>
# include <cstring>
# include <vector>
# include <regex>
# include <cassert>

# include <unistd.h>
# include <sys/wait.h>
# include <sys/epoll.h>

# include "../HTTP/HTTPRequest.hpp"
# include "../Webserv/SharedFd.hpp"
#include "CGIPipes.hpp"

# define CGI_ERR_RESPONSE "HTTP/1.1 500 Internal Server Error\nContent-Type: text/html\n\r\n<html>\n\
	<head><title>Server Error</title></head><body><h1>500: Internal Server Error</h1><h2>Something went wrong</h2></body></html>";

#ifndef READ
# define READ 0
#endif

#ifndef WRITE
# define WRITE 1
#endif

# define TIMEOUT 3

enum e_cgi_state {
	START_CGI,
	SEND_TO_CGI,
	RCV_FROM_CGI,
	CRT_RSPNS_CGI
};

class CGI {
	private:
		int						status_;
		bool					timeout_;
		bool					nph_;
		std::string 			scriptPath_;
		std::string 			response_;
		std::vector<std::string>envStrings_;
		CGIPipes				pipes_;
		pid_t					pid_;
		e_cgi_state				CGI_STATE_;
		time_t					start_time_;
		std::function<void(int)>delFromEpoll_cb_;
		const HTTPRequest&		request_;

		// START_CGI
		std::vector<char*>	createEnv();
		void				execCGI();

		// SEND_TO_CGI
		void				sendDataToCGI( const SharedFd &fd, uint32_t events );

		// RCV_FROM_CGI
		void				getResponseFromCGI( const SharedFd &fd, uint32_t events);
		int					getStatusCodeFromResponse( void );
		bool				isCGIProcessFinished( void );
		bool				isCGIProcessSuccessful( void );
		bool				hasCGIProcessTimedOut(void);


	public:
		explicit CGI(const HTTPRequest& request, CGIPipes pipes, std::function<void(int)> delFromEpoll_cb );
		~CGI( void );
		
		std::string					getResponse( void );
		bool						isReady( void );
		bool						isTimeout(void);
		void						handle( const SharedFd &fd, uint32_t events );
		void						rewriteResonseFromCGI( void );
		inline const std::string&	getScriptPath() const { return scriptPath_; }
		inline bool					isNPHscript() { return nph_;}
		
		static bool			isCGIScript( const std::string &path );
		static bool			isCGI(const HTTPRequest& request );
		static std::string	getScriptExecutable( const std::string &path );
		

		
};

class CGIException : public std::exception {
public:
	explicit CGIException(const std::string& message) : _message(message) {}
	const char* what() const noexcept override { return _message.c_str(); }
private:
	std::string _message;
};

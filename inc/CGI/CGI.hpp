#pragma once

# include <cstring>
# include <vector>
# include <cassert>
# include <cstring>
# include <string>

# include <unistd.h>
# include <sys/wait.h>
# include <sys/epoll.h>

# include "../HTTP/HTTPRequest.hpp"
# include "../Webserv/SharedFd.hpp"
# include "CGIPipes.hpp"

# define TIMEOUT 5

enum e_cgi_state {
	START_CGI,
	SEND_TO_CGI,
	RCV_FROM_CGI,
	HANDLE_RSPNS_CGI,
	CGI_DONE
};

class CGI {
	private:
		int						status_;
		bool					timeout_;
		bool					nph_;
		bool					finished_;
		std::string 			scriptPath_;
		std::string 			response_;
		std::vector<std::string>envStrings_;
		CGIPipes				pipes_;
		pid_t					pid_;
		e_cgi_state				CGI_STATE_;
		time_t					start_time_;
		std::function<void(int)>delFromEpoll_cb_;
		const HTTPRequest&		request_;
		std::string				send_data_;

		// START_CGI
		std::vector<char*>	createEnv();
		void				execCGI();

		// SEND_TO_CGI
		void				sendDataToCGI( const SharedFd &fd, uint32_t events );

		// RCV_FROM_CGI
		void				handleCGIResponse();
		void				getResponseFromCGI( const SharedFd &fd, uint32_t events);
		int					getStatusCodeFromResponse( void );
		bool				isCGIProcessFinished( void );
		bool				isCGIProcessSuccessful( void );
		void				handleTimeOut();


	public:
		explicit CGI(const HTTPRequest& request, CGIPipes pipes, \
			std::function<void(int)> delFromEpoll_cb);
		~CGI( void );
		
		std::string					getResponse( void );
		int							getStatusCode( void );
		bool						isDone( void );
		void						handle( const SharedFd &fd, uint32_t events );
		void						rewriteResponseFromCGI( void );
		inline const std::string&	getScriptPath() const { return scriptPath_; }
		inline bool					isNPHscript() { return nph_;}
		bool						timedOut(void);
		
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

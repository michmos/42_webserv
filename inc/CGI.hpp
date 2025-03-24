#pragma once

# include <iostream>
# include <cstring>
# include <vector>
# include <regex>

# include <unistd.h>
# include <sys/wait.h>
# include <sys/epoll.h>

# include "HTTPRequest.hpp"

# define READ 0
# define WRITE 1
# define TIMEOUT 10 // from configfile?

class CGI {
	private:
		std::string 		path_;
		int					pipe_to_child_[2];
		int					pipe_from_child_[2];
		pid_t				pid_;
		int					status_;
		std::string 		response_;
		const std::string	post_data_;
		epoll_event			epoll_event_pipe_[2];

		int				getStatusCodeFromResponse( void );

		// CGI UTILS
		void			createArgvVector( std::vector<char*> &argv_vector, const std::string &executable );
		void			createEnvCharPtrVector( std::vector<char*> &env_c_vector, std::vector<std::string> &env_vector );
		void			closeAllPipes( void);
		void			closeTwoPipes( int &pipe1, int &pipe2 );
		void			throwException( const char *msg );
		void			throwExceptionExit( const char *msg );

	public:
		explicit CGI( const std::string &post_data, std::vector<int> pipes );
		~CGI( void );

		void			forkCGI( const std::string &executable, std::vector<std::string> env_vector );
		std::string		receiveBuffer( int fd );
		void			sendDataToStdin( int fd );
		void			rewriteResonseFromCGI( void );
		
		// GETTERS
		std::string		getResponse( void );
		void			getResponseFromCGI( int fd );

		bool				isNPHscript( const std::string &executable );
		static bool			isCgiScript( const std::string &path );
		static std::string	getScriptExecutable( const std::string &path );
		std::vector<char*>	createEnv( std::vector<std::string> &envStrings, const HTTPRequest request );

		void			watchDog( void );
};

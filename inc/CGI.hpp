#pragma once

# include <poll.h>
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
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
# include <sys/types.h>
# include <sys/wait.h>
# include <csignal>
# include <atomic>
# include <utility>
# include <ctime>
# include <chrono>
# include <thread>
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

	public:
		CGI( const std::string &post_data, std::vector<int> pipes );
		~CGI( void );

		void			forkCGI( const std::string &executable, std::vector<std::string> env_vector );
		std::string		receiveBuffer( void );
		void			sendDataToStdin( const std::string &post_data );
		void			createArgvVector( std::vector<char*> &argv_vector, const std::string &executable );
		void			createEnvCharPtrVector( std::vector<char*> &env_c_vector, std::vector<std::string> &env_vector );
		void			rewriteResonseFromCGI( void );
		
		// GETTERS
		std::string		getResponse( void );
		int				getStatusCodeFromResponse( void );
		void			getResponseFromCGI( void );

		bool				isNPHscript( const std::string &executable );
		static bool			isCgiScript( const std::string &path );
		static std::string	getScriptExecutable( const std::string &path );
		std::vector<char*>	createEnv( std::vector<std::string> &envStrings, const HTTPRequest request );

		void			watchDog( void );

		// CGI UTILS
		void			setPipes( void );
		void			closeAllPipes( void);
		void			closeTwoPipes( int &pipe1, int &pipe2 );
		void			throwException( const char *msg );
		void			throwExceptionExit( const char *msg );
};

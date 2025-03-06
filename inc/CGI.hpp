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

#define READ 0
#define WRITE 1

class CGI {
	private:
		std::string m_path;
		int			m_pipe_to_child[2];
		int			m_pipe_from_child[2];
		pid_t		m_pid;
		int			m_status;
		std::string m_response;

	public:
		CGI( const std::string &script_path, std::vector<std::string> env_vector, const std::string &post_data);
		~CGI( void );

		std::string		receiveBuffer( void );
		void			sendDataToStdin( const std::string &post_data );
		void			createArgvVector( std::vector<char*> &argv_vector, const std::string &executable );
		void			createEnvCharPtrVector( std::vector<char*> &env_c_vector, std::vector<std::string> &env_vector );
		void			rewriteResonseFromCGI( void );
		void			waitForChild( void );
		
		// GETTERS
		std::string		getResponse( void );
		int				getStatusCodeFromResponse( void );
		void			getResponseFromCGI( void );

		bool				isNPHscript( const std::string &executable );
		static bool			isCgiScript( const std::string &path );
		static std::string	getScriptExecutable( const std::string &path );

		// CGI UTILS
		void			setPipes(void);
		void			closeAllPipes(void);
		void			closeTwoPipes(int &pipe1, int &pipe2);
		void			throwException(const char *msg );
		void			throwExceptionExit(const char *msg );
};

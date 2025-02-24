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
# include <sys/types.h>
# include <sys/wait.h>
# include <csignal>
# include <atomic>
# include <utility>

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

		void			send_body_to_stdin( const std::string &buffer );
		std::string		receive_buffer( void );
		std::string		get_response_CGI( void );

		static bool			is_cgi_script( const std::string &path );
		static std::string	get_script_executable( const std::string &path );
};
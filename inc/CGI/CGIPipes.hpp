#pragma once

# include <vector>
# include <functional>
# include <cstring>

# include <sys/types.h>
# include <fcntl.h>

# include "../Webserv/Epoll.hpp"

#ifndef WRITE
# define WRITE 1
#endif

#ifndef READ
# define READ 0
#endif

#ifndef FROM_CGI_READ
# define FROM_CGI_READ 0
#endif

#ifndef TO_CGI_WRITE
# define TO_CGI_WRITE 3
#endif

class CGIPipes {
	private:
		std::vector<int>											pipes_;
		std::function<void(struct epoll_event, const SharedFd&)>	pipe_callback_;
		SharedFd													client_fd_;

	public:
		CGIPipes( void );
		~CGIPipes( void );

		void				setCallbackFunction( std::function<void(struct epoll_event, \
								const SharedFd&)>  callback, const SharedFd& server_fd );
		
		void				addNewPipes( void );
		void				addPipesToEpoll( void );
		std::vector<int>	getPipes( void );
};

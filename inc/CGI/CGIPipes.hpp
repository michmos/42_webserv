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
		std::vector<SharedFd>										pipes_;
		std::function<void(struct epoll_event, const SharedFd&)>	pipe_add_cb_;
		std::function<void(const SharedFd&)>						pipe_remove_cb_;
		SharedFd													client_fd_;

	public:
		CGIPipes( void );
		~CGIPipes( void );

		void				setCallbackFunctions( const SharedFd& client_fd, \
								std::function<void(struct epoll_event, const SharedFd&)> pipe_add_cb, \
								std::function<void(const SharedFd&)> pipe_remove_cb );
		
		void				addNewPipes( void );
		void				addPipesToEpoll( void );
		void				deletePipesFromEpoll(int &fd);
		std::vector<SharedFd>	getPipes( void );
};

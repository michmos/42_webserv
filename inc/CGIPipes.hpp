#pragma once

# include "Epoll.hpp"
# include <vector>
# include <sys/types.h>
# include <fcntl.h>
# include <functional>
# include <cstring>

# define WRITE 0
# define READ 1
# define FROM_CHILD_READ 0
# define TO_CHILD_WRITE 3

class CGIPipes {
	private:
		std::vector<std::vector<int>>	pipes_;
		std::function<void(int, int)>	pipe_callback_;

	public:
		CGIPipes( void );
		~CGIPipes( void );

		void				setCallbackFunction( std::function<void(int, int)> callback );
		
		void				addNewPipes( void );
		void				addPipesToEpoll( void );
		void				closeAllPipes( void );
		std::vector<int>&	getLastPipes( void );
};

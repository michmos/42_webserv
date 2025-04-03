#include "../../inc/CGI/CGIPipes.hpp"

CGIPipes::CGIPipes(void) {
	pipes_.resize(4, -1);
}

CGIPipes::~CGIPipes(void) { closeAllPipes(); }

void	CGIPipes::setCallbackFunction(std::function<void(struct epoll_event, const SharedFd&)> callback, \
	const SharedFd& server_fd) {
	pipe_callback_ = callback;
	server_fd_ = server_fd;
}

std::vector<int>	CGIPipes::getPipes(void) { return (pipes_); }

/// @brief set pipes to -1 and then open both (to and from child) + nonblock
void	CGIPipes::addNewPipes(void) {
	int					pipe_from_child[2];
	int					pipe_to_child[2];

	std::memset(pipe_from_child, -1, sizeof(pipe_from_child));
	std::memset(pipe_to_child, -1, sizeof(pipe_to_child));
	if (pipe(pipe_to_child) < 0)
	{
		std::perror("pipe_from_child failed");
		throw std::exception();
	}
	if (pipe(pipe_from_child) < 0)
	{
		std::perror("pipe_to_child failed");
		close(pipe_to_child[WRITE]);
		close(pipe_to_child[READ]);
		throw std::exception();
	}
	fcntl(pipe_to_child[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(pipe_to_child[READ], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_child[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_child[READ], F_SETFL, O_NONBLOCK);
	pipes_[0] = pipe_from_child[READ];
	pipes_[1] = pipe_from_child[WRITE];
	pipes_[2] = pipe_to_child[READ];
	pipes_[3] = pipe_to_child[WRITE];
	addPipesToEpoll();
	std::cerr << "add new pipes5" << std::endl;
}

/// @brief takes the last pipe and exectutes the pipe_callback function
void	CGIPipes::addPipesToEpoll() {
	epoll_event			event_write;
	epoll_event 		event_read;

	event_write.data.fd = pipes_[TO_CHILD_WRITE];
	event_write.events = EPOLLOUT;
	pipe_callback_(event_write, server_fd_);
	event_read.data.fd = pipes_[FROM_CHILD_READ];
	event_read.events = EPOLLIN;
	pipe_callback_(event_read, server_fd_);
}

/// @brief closes all pipes that are stored in a vector<vector<int>> array
void	CGIPipes::closeAllPipes(void) {
	for (size_t j = 0; j < pipes_.size(); j++)
	{
		if (pipes_[j] != -1)
		{
			close(pipes_[j]);
			pipes_[j] = -1;
		}
	}
}

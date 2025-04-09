#include "../../inc/CGI/CGIPipes.hpp"

CGIPipes::CGIPipes(void) {
	pipes_.resize(4, -1);
}

CGIPipes::~CGIPipes(void) {}

void	CGIPipes::setCallbackFunctions( const SharedFd& client_fd, \
		std::function<void(struct epoll_event, const SharedFd&)> pipe_add_cb, \
		std::function<void(const SharedFd&)> pipe_remove_cb ) {
	client_fd_ = client_fd;
	pipe_add_cb_ = pipe_add_cb;
	pipe_remove_cb_ = pipe_remove_cb;
}

std::vector<SharedFd>	CGIPipes::getPipes(void) { return (pipes_); }

/// @brief set pipes to -1 and then open both (to and from child) + nonblock
void	CGIPipes::addNewPipes(void) {
	int	pipe_from_cgi[2];
	int	pipe_to_cgi[2];

	std::memset(pipe_from_cgi, -1, sizeof(pipe_from_cgi));
	std::memset(pipe_to_cgi, -1, sizeof(pipe_to_cgi));
	if (pipe(pipe_to_cgi) < 0)
	{
		std::perror("pipe_from_cgi failed");
		throw std::exception();
	}
	if (pipe(pipe_from_cgi) < 0)
	{
		std::perror("pipe_to_cgi failed");
		pipe_to_cgi[WRITE] = -1;
		pipe_to_cgi[READ] = -1;
		throw std::exception();
	}
	if (fcntl(pipe_to_cgi[WRITE], F_SETFL, O_NONBLOCK) == -1)
		std::perror("fcntl fails with pipes\n");
	if (fcntl(pipe_to_cgi[READ], F_SETFL, FD_CLOEXEC) == -1)
		std::perror("fcntl fails with pipes\n");
	if (fcntl(pipe_from_cgi[WRITE], F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		std::perror("fcntl fails with pipes\n");
	if (fcntl(pipe_from_cgi[READ], F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		std::perror("fcntl fails with pipes\n");
	pipes_[0] = pipe_from_cgi[READ];
	pipes_[1] = pipe_from_cgi[WRITE];
	pipes_[2] = pipe_to_cgi[READ];
	pipes_[3] = pipe_to_cgi[WRITE];
	addPipesToEpoll();
}

/// @brief takes the last pipe and exectutes the pipe_callback function
void	CGIPipes::addPipesToEpoll(void) {
	epoll_event			event_write;
	epoll_event 		event_read;

	event_write.data.fd = pipes_[TO_CGI_WRITE].get();
	event_write.events = EPOLLOUT | O_NONBLOCK;
	pipe_add_cb_(event_write, client_fd_);
	event_read.data.fd = pipes_[FROM_CGI_READ].get();
	event_read.events =  EPOLLIN | O_NONBLOCK;
	pipe_add_cb_(event_read, client_fd_);
}

void	CGIPipes::deletePipesFromEpoll(SharedFd &fd) {
	if (fd == pipes_[TO_CGI_WRITE].get())
	{
		pipe_remove_cb_(pipes_[TO_CGI_WRITE]);
		pipes_[TO_CGI_WRITE] = -1;
		fd = -1;
	}
	else if (fd == pipes_[FROM_CGI_READ].get())
	{
		pipe_remove_cb_(pipes_[FROM_CGI_READ]);
		pipes_[FROM_CGI_READ] = -1;
		fd = -1;
	}
}

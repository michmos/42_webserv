#include "../../inc/CGI/CGIPipes.hpp"

CGIPipes::CGIPipes(void) {
	pipes_.resize(4, -1);
}

CGIPipes::~CGIPipes(void) { closeAllPipes(); }

void	CGIPipes::setCallbackFunction(std::function<void(struct epoll_event, const SharedFd&)> callback, \
	const SharedFd& client_fd) {
	pipe_callback_ = callback;
	client_fd_ = client_fd;
}

std::vector<int>	CGIPipes::getPipes(void) { return (pipes_); }

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
		close(pipe_to_cgi[WRITE]);
		close(pipe_to_cgi[READ]);
		std::cerr << "close pipes: " << pipe_to_cgi[WRITE] << " and " << pipe_to_cgi[READ] << std::endl;
		throw std::exception();
	}
	fcntl(pipe_to_cgi[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(pipe_to_cgi[READ], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_cgi[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_cgi[READ], F_SETFL, O_NONBLOCK);
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

	event_write.data.u32 = client_fd_.get();
	event_write.data.fd = pipes_[TO_CGI_WRITE];
	event_write.events = EPOLLOUT;
	std::cerr << "add pipes: " << event_write.data.fd << std::endl;
	pipe_callback_(event_write, client_fd_);
	event_read.data.u32 = client_fd_.get();
	event_read.data.fd = pipes_[FROM_CGI_READ];
	event_read.events = EPOLLIN;
	std::cerr << "add pipes: " << event_read.data.fd << std::endl;
	pipe_callback_(event_read, client_fd_);
}

/// @brief closes all pipes that are stored in a vector<vector<int>> array
void	CGIPipes::closeAllPipes(void) {
	for (size_t j = 0; j < pipes_.size(); j++)
	{
		if (pipes_[j] != -1)
		{
			close(pipes_[j]);
			std::cerr << "close pipe : " << pipes_[j] << std::endl;
			pipes_[j] = -1;
		}
	}
}

#include "../../inc/CGI/CGIPipes.hpp"

CGIPipes::CGIPipes( void ) {};

CGIPipes::~CGIPipes(void) {}

SharedFd& CGIPipes::operator[](size_t i) {
	if (i > 3) {
		throw std::invalid_argument("CGIPipes operator[]: " + std::to_string(i));
	}
	return (pipes_[i]);
}

void	CGIPipes::setCallbackFunctions( const SharedFd& client_fd, \
		std::function<void(struct epoll_event, const SharedFd&)> pipe_add_cb, \
		std::function<void(const SharedFd&)> pipe_remove_cb ) {
	client_fd_ = client_fd;
	pipe_add_cb_ = pipe_add_cb;
	pipe_remove_cb_ = pipe_remove_cb;
}

/// @brief open pipes and set flags
/// @THROW throws exception if syscall fails
void	CGIPipes::addNewPipes(void) {
	try {
		// create pipes
		auto addPipe = [this](){
			int fds[2];
			if (::pipe(fds) == -1) {
				throw std::runtime_error("pipe(): " + std::string(strerror(errno)));
			}
			pipes_.push_back(fds[0]);
			pipes_.push_back(fds[1]);
		};
		addPipe();
		addPipe();

		// set NONBLOCK and CLOEXEC flags
		auto setFlags = [](SharedFd& fd) {
			int statusFlags = fcntl(fd.get(), F_GETFL);
			if (statusFlags == -1)
				throw std::runtime_error("fcntl(F_GETFL): " + std::string(strerror(errno)));
			if (fcntl(fd.get(), F_SETFL, statusFlags | O_NONBLOCK) == -1)
				throw std::runtime_error("fcntl(F_SETFL): " + std::string(strerror(errno)));
			if (fcntl(fd.get(), F_SETFD, FD_CLOEXEC) == -1)
				throw std::runtime_error("fcntl(F_SETFD): " + std::string(strerror(errno)));
		};
		for (auto fd: pipes_) {
			setFlags(fd);
		}
	}
	catch (std::runtime_error &e)
	{
		pipes_.clear();
		throw;
	}
	// add fd to epoll
	auto addToEpoll = [this](int fd, u_int32_t events) {
		epoll_event	ev;
		ev.data.fd = fd;
		ev.events = events;

		pipe_add_cb_(ev, client_fd_);
	};
	addToEpoll(pipes_[TO_CGI_WRITE].get(), EPOLLOUT);
	addToEpoll(pipes_[FROM_CGI_READ].get(), EPOLLIN);
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

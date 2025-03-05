
#include "../inc/SharedFd.hpp"
#include <stdexcept>

UniqueFd::UniqueFd(int fd) : _fd(fd) {
	#ifdef DEBUG
	std::cout << "UniqueFd Instance created" << std::endl;
	#endif
}

UniqueFd::~UniqueFd() {
	close(_fd);
	#ifdef DEBUG
	std::cout << "UniqueFd Instance destroyed" << std::endl;
	#endif
}

int	UniqueFd::getFd() const {
	return (_fd);
}

SharedFd::SharedFd() : _fd(nullptr) {
}

SharedFd::SharedFd(int fd) {
	_fd = std::make_shared<UniqueFd>(fd);
}

SharedFd::SharedFd(const SharedFd& other) {
	if (this != &other) {
		*this = other;
	}
}

SharedFd& SharedFd::operator=(const SharedFd& other) {
	this->_fd = other._fd;
	return(*this);
}

// TODO: maybe delete this overload - see whether actually makes sense
SharedFd& SharedFd::operator=(int fd) {
	this->_fd = std::make_shared<UniqueFd>(fd);
	return(*this);
}

bool	SharedFd::operator==(const SharedFd& other) {
	if (this->_fd == other._fd)
		return (true);
	return (false);
}

SharedFd::~SharedFd() {
}

void	SharedFd::setNonBlock() const {
	if (!_fd)
		throw std::invalid_argument("setNonBlock(): _fd not set");
	int	fd = _fd->getFd();
	int flags = fcntl(fd, F_GETFL);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl()") + strerror(errno));
}

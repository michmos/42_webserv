
#include "../inc/SharedFd.hpp"
#include <memory>

UniqueFd::UniqueFd(int fd) : _fd(fd) {
	#ifdef DEBUG
	std::cout << "UniqueFd Instance created" << std::endl;
	#endif
}

UniqueFd::~UniqueFd() {
	if (_fd > 0)
		close(_fd);
	#ifdef DEBUG
	std::cout << "UniqueFd Instance destroyed" << std::endl;
	#endif
}

int	UniqueFd::get() const {
	return (_fd);
}

SharedFd::SharedFd() : _fd(std::make_shared<UniqueFd>(-1)) {
}

SharedFd::SharedFd(int fd) {
	if (fd < 0)
		throw std::invalid_argument("SharedFd(int): fd < 0");
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
	return (this->_fd == other._fd);
}

SharedFd::~SharedFd() {
}

void	SharedFd::setNonBlock() const {
	if (!this->isValid())
		throw std::invalid_argument("setNonBlock(): _fd not set");
	int	fd = this->get();
	int flags = fcntl(fd, F_GETFL);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl()") + strerror(errno));
}

bool	SharedFd::isValid() const {
	return (_fd->get() >= 0);
}

int	SharedFd::get() const {
	if (!this->isValid())
		throw std::runtime_error("SharedFd.get(): _fd not set");
	return(_fd->get());

}

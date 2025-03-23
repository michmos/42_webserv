
#include "../inc/SharedFd.hpp"

UniqueFd::UniqueFd(int fd) : _fd(fd) {
}

UniqueFd::~UniqueFd() {
	if (_fd > 0)
		close(_fd);
}

int	UniqueFd::get() const {
	return (_fd);
}

SharedFd::SharedFd() : _fdPtr(std::make_shared<UniqueFd>(-1)) {
}

SharedFd::SharedFd(int fd) : _fdPtr(std::make_shared<UniqueFd>(fd)) {
}

SharedFd::SharedFd(const SharedFd& other) {
	if (this != &other) {
		*this = other;
	}
}

// assignment operators
SharedFd& SharedFd::operator=(const SharedFd& other) {
	this->_fdPtr = other._fdPtr;
	return(*this);
}

SharedFd& SharedFd::operator=(int fd) {
	this->_fdPtr = std::make_shared<UniqueFd>(fd);
	return(*this);
}

// comparison operators
bool	SharedFd::operator==(const SharedFd& other) {
	return (this->get() == other.get());
}

bool	SharedFd::operator<(const SharedFd& other) {
	return (this->get() < other.get());
}

bool	SharedFd::operator>(const SharedFd& other) {
	return (this->get() > other.get());
}

bool	SharedFd::operator<=(const SharedFd& other) {
	return (this->get() <= other.get());
}

bool	SharedFd::operator>=(const SharedFd& other) {
	return (this->get() >= other.get());
}

SharedFd::~SharedFd() {
}

// TODO: forbidden flag
void	SharedFd::setNonBlock() const {
	if (!this->isValid())
		throw std::invalid_argument("setNonBlock(): _fd not set");
	int	fd = this->get();
	int flags = fcntl(fd, F_GETFL);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("fcntl()") + strerror(errno));
}

bool	SharedFd::isValid() const {
	return (this->get() >= 0);
}

int	SharedFd::get() const {
	return(_fdPtr->get());
}

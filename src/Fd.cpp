
#include "../inc/Fd.hpp"

// constructors
Fd::Fd() : _fd(-1) {
}

Fd::Fd(int fd) : _fd(fd) {
}

// copy constructor
Fd::Fd(const Fd& fd) {
	*this = fd;
}

// assignment operators
Fd& Fd::operator=(const Fd& toCopy) {
	if (this != &toCopy) {
		if (_fd != -1)
			close(_fd);
		_fd = toCopy.getFd();
	}
	return (*this);
}

Fd& Fd::operator=(int fd) {
	if (_fd != -1)
		close(_fd);

	_fd = fd;
	return (*this);
}

// comparison operators
bool Fd::operator==(int fd) const {
	return (this->_fd == fd);
}

bool Fd::operator==(const Fd& other) const {
	return (this->_fd == other.getFd());
}

int	Fd::getFd() const {
	return (_fd);
}

Fd::~Fd() {
	if (_fd != -1)
		close(_fd);
}

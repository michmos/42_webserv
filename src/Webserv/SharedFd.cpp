
#include "../../inc/Webserv/SharedFd.hpp"
#include <iomanip>

std::unordered_map<int, int> SharedFd::_refCounts;

SharedFd::SharedFd() : SharedFd(UNVALID_FD) {
}

SharedFd::SharedFd(int fd) : _fd(fd) {
	_refCounts[fd]++;
}

SharedFd::SharedFd(const SharedFd& other) : SharedFd() {
	if (this != &other) {
		*this = other;
	}
}

SharedFd& SharedFd::operator=(const SharedFd& other) {
	return (*this = other._fd);
}

SharedFd& SharedFd::operator=(int fd) {
	if (this->_fd != fd) {
		_refCounts[this->_fd]--;
		this->_fd = fd;
		_refCounts[this->_fd]++;
	}
	return (*this);
}

void	SharedFd::printOpenFds() {
	std::cout << std::left << std::setw(10) << "fd" <<  "count" << std::endl;
	for (auto it : _refCounts) {
		if (it.second > 0) {
			std::cout << std::left << std::setw(10) << it.first << it.second << std::endl;
		}
	}
}

// TODO: potentially rmv err_msg 
SharedFd::~SharedFd() {
	_refCounts[_fd]--;
	if (_refCounts[_fd] == 0 && _fd >= 0)
	{
		if(close(_fd) == -1)
			std::cerr << "close() " << _fd << " : " << strerror(errno) << std::endl;
	}
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


#include "../inc/Client.hpp"
#include <stdexcept>


Client::Client(int fd) : _fd(fd), _status(DFLT) {
}

tStatus Client::getStatus() const {
	return (_status);
}

void Client::setStatus(tStatus newStatus) {
	_status = newStatus;
}

int Client::getFd() const {
	return (_fd);
}

void	Client::readFrom() {
	this->setStatus(READING);

	char buff[READSIZE + 1] = { '\0'};
	int bRead = read(_fd, buff, READSIZE);
	if (bRead == -1) {
		throw std::runtime_error(std::string("read(): ") + strerror(errno));
	}
	_buff.append(buff);

	if (bRead < READSIZE) {
		this->setStatus(FINISHED_READING);
		#ifdef DEBUG
		std::cout << "Full string: " << _buff << std::endl;
		#endif
	}
	#ifdef DEBUG
	std::cout << "Received: " << buff << std::endl;
	#endif
}

void	Client::writeTo() {
}


Client::~Client() {
}

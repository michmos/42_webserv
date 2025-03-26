
#include "../inc/Client.hpp"


Client::Client(int fd) : _fd(fd), _state(DFLT) {
}

tState Client::getState() const {
	return (_state);
}

void Client::setState(tState newState) {
	_state = newState;
}

int Client::getFd() const {
	return (_fd);
}

void	Client::readFrom() {
	this->setState(READING);

	char buff[READSIZE + 1] = { '\0'};
	int bRead = read(_fd, buff, READSIZE);
	if (bRead == -1) {
		throw std::runtime_error(std::string("read(): ") + strerror(errno));
	}
	_buff.append(buff);

	if (bRead < READSIZE) {
		this->setState(FINISHED_READING);
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

#include "../inc/Server.hpp"

Server::Server(int fd) : _fd(fd) {
}

Server::~Server() {
}

int	Server::getFd() const {
	return(_fd);
}

const std::string& Server::getName() const {
	return(_name);
}

void	Server::setName(const std::string& name) {
	_name = name;
}



#include "../inc/Socket.hpp"
#include <stdexcept>
#include <string>

Socket::Socket() : 
	_fd(socket(AF_INET, SOCK_STREAM, 0))
{
	if (_fd == -1) {
		throw std::runtime_error(std::string("socket(): ") + strerror(errno));
	}
}

Socket::Socket(int sockFd) : 
	_fd(sockFd)
{
	if (_fd < 0) {
		throw std::invalid_argument(std::string("Socket(int) constructor called with ") + std::to_string(sockFd));
	}
}

Socket::~Socket() {
	close(_fd);
}

void	Socket::sBind(u_int32_t ipv4Addr, u_int16_t port) const {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ipv4Addr;
	addr.sin_port = htons(port);

	if (bind(_fd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
		throw std::runtime_error(std::string("bind(): ") + strerror(errno));
	}
}

void	Socket::sListen(int backlog) const {
	if (listen(_fd, backlog) == -1) {
		throw std::runtime_error(std::string("listen(): ") + strerror(errno));
	}
}

int	Socket::sAccept() const {
	int	newClient;

	if ((newClient = accept(_fd, nullptr, nullptr)) == -1) {
		throw std::runtime_error(std::string("accept(): ") + strerror(errno));
	}
	return (newClient);
}

void	Socket::sConnect(u_int32_t ipv4Addr, u_int16_t port) const {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ipv4Addr;
	addr.sin_port = htons(port);

	if (connect(_fd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
		throw std::runtime_error(std::string("connect(): ") + strerror(errno));
	}
}

int	Socket::getFd() const {
	return (_fd);
}


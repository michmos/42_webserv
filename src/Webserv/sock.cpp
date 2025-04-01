
#include "../../inc/Webserv/sock.hpp"

int sock::create() {
	int fd = ::socket(AF_INET,SOCK_STREAM, 0);
	if (fd == -1) {
		throw std::runtime_error(std::string("socket(): ") + strerror(errno));
	}
	return (fd);
}

void	sock::bind(int fd, in_addr_t ipv4Addr, in_port_t port) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ipv4Addr;
	addr.sin_port = port;

	if (::bind(fd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
		throw std::runtime_error(std::string("bind(): ") + strerror(errno));
	}
}

void	sock::listen(int fd, int backlog) {
	if (::listen(fd, backlog) == -1) {
		throw std::runtime_error(std::string("listen(): ") + strerror(errno));
	}
}

int	sock::accept(int fd) {
	int	newClient;

	if ((newClient = ::accept(fd, nullptr, nullptr)) == -1) {
		throw std::runtime_error(std::string("accept(): ") + strerror(errno));
	}
	return (newClient);
}

void	sock::connect(int fd, in_addr_t ipv4Addr, in_port_t port) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ipv4Addr;
	addr.sin_port = port;

	if (::connect(fd, (const struct sockaddr*)&addr, sizeof(addr)) == -1) {
		throw std::runtime_error(std::string("connect(): ") + strerror(errno));
	}
}


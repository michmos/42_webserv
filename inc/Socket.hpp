
#pragma once

#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

// TODO: use Namesapce instead of class
//
// use only for for IPv4 stream sockets
class Sockets {
public:
	Sockets() = delete;
	Sockets(const Sockets&) = delete;
	Sockets& operator=(const Sockets&) = delete;
	~Sockets() = delete;

	static void	sBind(int fd, in_addr_t addr, in_port_t port);
	static void	sListen(int fd, int backlog);
	static int	sAccept(int fd);
	static void	sConnect(int fd, in_addr_t addr2, in_port_t port);
	static int	getFd(int fd);
};

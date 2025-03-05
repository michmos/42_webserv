
#pragma once

#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

// use only for for IPv4 stream sockets
class Socket {
public:
	Socket();
	Socket(int sockFd);
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	~Socket();

	void	sBind(in_addr_t addr, in_port_t port) const;
	void	sListen(int backlog) const;
	int		sAccept() const;
	void	sConnect(in_addr_t addr2, in_port_t port) const;
	int		getFd() const;
	void	setNonBlock() const;

private:
	int	_fd;
};


#pragma once

#include "SharedFd.hpp"
#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <netinet/in.h>
#include <fcntl.h>


// use only for for IPv4 stream sockets
class Socket {
public:
	Socket();
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	~Socket();

	void			bind(in_addr_t addr, in_port_t port);
	void			listen(int backlog);
	int				accept();
	void			connect(in_addr_t addr2, in_port_t port);
	const SharedFd&	getFd() const;

private:
	SharedFd	_fd;
};

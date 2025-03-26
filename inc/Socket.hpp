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
	Socket(int sockFd);
	Socket& operator=(int sockFd);
	Socket(const Socket&);
	Socket& operator=(const Socket&);
	~Socket();


	// - addr: e.g. inet_addr("127.0.0.1")
	// - port: e.g. htons(8080)
	void			bind(in_addr_t addr, in_port_t port);
	void			listen(int backlog);
	int				accept();
	// - addr: e.g. inet_addr("127.0.0.1")
	// - port: e.g. htons(8080)
	void			connect(in_addr_t addr2, in_port_t port);
	inline const SharedFd&	getFd() const { return (_fd); }

private:
	SharedFd	_fd;
};
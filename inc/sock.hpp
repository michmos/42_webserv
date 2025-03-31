
#pragma once

#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <netinet/in.h>
#include <fcntl.h>


// use only for for IPv4 stream sockets
namespace sock {
	int				create();
	// - addr: e.g. inet_addr("127.0.0.1")
	// - port: e.g. htons(8080)
	void			bind(int fd, in_addr_t addr, in_port_t port);
	void			listen(int fd, int backlog);
	int				accept(int fd);
	// - addr: e.g. inet_addr("127.0.0.1")
	// - port: e.g. htons(8080)
	void			connect(int fd, in_addr_t addr2, in_port_t port);
}

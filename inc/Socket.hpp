
#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <sys/socket.h>
#include <string.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

// use only for for IPv4 stream sockets
class Socket {
public:
	Socket();
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	~Socket();
	void	sBind(u_int32_t addr, u_int16_t port);
	void	sListen(int backlog);
	void	sAccept();
	void	sConnect(u_int32_t addr2, u_int16_t port);
private:
	int	_fd;
};

#endif

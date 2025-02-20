
#ifndef EPOLL_HPP
# define EPOLL_HPP

#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>
#include <unistd.h>

#define	TIMEOUT_DFLT 3
#define	MAX_EVENTS 5

class Epoll {
public:
	Epoll(int size);
	Epoll(const Epoll &toCopy) = delete;
	Epoll& operator=(const Epoll& toAssign) = delete;
	~Epoll() noexcept;

	void	add(int fd, u_int32_t events) const;
	void	mod(int fd, u_int32_t events) const;
	void	del(int fd) const;
	void	wait();
	void	setTimeout(int timeout) noexcept;

private:
	int					_epFd;
	int					_timeout;
	static const int	_maxEvents = MAX_EVENTS;
	struct epoll_event _events[_maxEvents];
};

#endif

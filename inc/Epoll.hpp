
#ifndef EPOLL_HPP
# define EPOLL_HPP

#include <sys/types.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdexcept>
#include <unistd.h>
#include <vector>

#define	TIMEOUT_DFLT 3
#define	MAX_EVENTS 5

// since Linux 2.6.8 ignored anyways
#define	EPOLL_START_SIZE 5

class Epoll {
public:
	Epoll();
	Epoll(const Epoll &toCopy) = delete;
	Epoll& operator=(const Epoll& toAssign) = delete;
	~Epoll() noexcept;

	void	add(int fd, u_int32_t events) const;
	void	mod(int fd, u_int32_t events) const;
	void	del(int fd) const;
	const std::vector<struct epoll_event>&	wait();
	void	setTimeout(int timeout) noexcept;
	int		getEpFd() const;

	// TODO: maybe add pwait() from epoll_pwait()

private:
	int								_epFd;
	int								_timeout;
	std::vector<struct epoll_event>	_events;
};

#endif

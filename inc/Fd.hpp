
#ifndef FD_HPP
# define FD_HPP

#include <unistd.h>

class Fd {
public:
	Fd();
	Fd(int newFd);
	Fd(const Fd& fd);
	Fd& operator=(const Fd& toCopy);
	Fd& operator=(int fd);
	bool operator==(const Fd& other) const;
	bool operator==(int fd) const;
	~Fd();

	int	getFd() const;
	bool	isOpen() const;
private:
	int		_fd;
	bool	_open;
};

#endif

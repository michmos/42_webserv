
#pragma once

#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <unordered_map>
#include <stdexcept>

class	SharedFd {
public:
	SharedFd();
	SharedFd(int fd);
	SharedFd(const SharedFd& other);
	SharedFd& operator=(const SharedFd& other);
	SharedFd& operator=(int fd);
	bool	operator==(const SharedFd& other);
	bool	operator<(const SharedFd& other);
	bool	operator>(const SharedFd& other);
	bool	operator<=(const SharedFd& other);
	bool	operator>=(const SharedFd& other);
	~SharedFd();

	bool	isValid() const;
	void	setNonBlock() const;
	int		get() const;

private:
	int	_fd;
	static std::unordered_map<int, int> _refCounts;
};

// special hash template struct to allow SharedFd as key in unordered map
template<>
struct std::hash<SharedFd> {
	size_t	operator()(const SharedFd& fd) const {
		return(std::hash<int>()(fd.get()));
	}
};

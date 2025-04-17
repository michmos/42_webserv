#pragma once

#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <unordered_map>
#include <iomanip>
#include <stdexcept>
#include <iomanip>

#define UNVALID_FD -1

class	SharedFd {
public:
	SharedFd();
	SharedFd(int fd);
	SharedFd(const SharedFd& other);
	SharedFd& operator=(const SharedFd& other);
	SharedFd& operator=(int fd);
	~SharedFd();

	// comparison operators
	inline bool	operator==(const SharedFd& other) const { return (this->_fd == other._fd); }
	inline bool	operator!=(const SharedFd& other) const { return (this->_fd != other._fd); }
	inline bool	operator<(const SharedFd& other) const { return (this->_fd < other._fd); }
	inline bool	operator>(const SharedFd& other) const { return (this->_fd > other._fd); }
	inline bool	operator<=(const SharedFd& other) const { return (this->_fd <= other._fd); }
	inline bool	operator>=(const SharedFd& other) const { return (this->_fd >= other._fd); }
	inline bool	operator==(int other) const { return (this->_fd == other); }
	inline bool	operator!=(int other) const { return (this->_fd != other); }
	inline bool	operator<(int other) const { return (this->_fd < other); }
	inline bool	operator>(int other) const { return (this->_fd > other); }
	inline bool	operator<=(int other) const { return (this->_fd <= other); }
	inline bool	operator>=(int other) const { return (this->_fd >= other); }

	// conversion operator
	inline explicit operator	int() const { return (_fd); }

	inline bool	isValid() const { return (this->_fd >= 0); }
	inline int	get() const { return (_fd); }
	static void	printOpenFds();

private:
	int	_fd;
	static std::unordered_map<int, int> _refCounts;
	void	closeFd();
};

// special hash template struct to allow SharedFd as key in unordered map
template<>
struct std::hash<SharedFd> {
	size_t	operator()(const SharedFd& fd) const {
		return(std::hash<int>()(fd.get()));
	}
};

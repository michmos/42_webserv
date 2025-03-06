
#pragma once
#include <memory>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <exception>

class	UniqueFd {
public:
	UniqueFd(int fd);
	UniqueFd(const UniqueFd& other) = delete;
	UniqueFd& operator=(const UniqueFd& other) = delete;
	~UniqueFd();

	int	get() const;
private:
	int	_fd;
};

class	SharedFd {
public:
	SharedFd();
	SharedFd(int fd);
	SharedFd(const SharedFd& other);
	SharedFd& operator=(const SharedFd& other);
	SharedFd& operator=(int fd);
	bool	operator==(const SharedFd& other);
	~SharedFd();

	bool	isValid() const;
	void	setNonBlock() const;
	int		get() const;

private:
	std::shared_ptr<UniqueFd>	_fd;
};

// special hash template struct to allow SharedFd as key in unordered map
template<>
struct std::hash<SharedFd> {
	size_t	operator()(const SharedFd& fd) const {
		return(std::hash<int>()(fd.get()));
	}
};

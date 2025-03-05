
#pragma once
#include <memory>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <stdexcept>

class	UniqueFd {
public:
	UniqueFd(int fd);
	UniqueFd(const UniqueFd& other) = delete;
	UniqueFd& operator=(const UniqueFd& other) = delete;
	~UniqueFd();

	int	getFd() const;
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

	void	setNonBlock() const;

private:
	std::shared_ptr<UniqueFd>	_fd;
};

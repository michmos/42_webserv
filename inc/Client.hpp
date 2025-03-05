
#pragma once

#include <iostream>
#include <unistd.h>
#include <string.h>
#define READSIZE	100

typedef enum eStatus {
	DFLT,
	READING,
	FINISHED_READING,
	WRITING,
	FINISHED_WRITING,
	FINISHED
} tStatus;

class Client {
public:
	Client(int fd);
	~Client();

	void	readFrom();
	void	writeTo();

	tStatus	getStatus() const;
	void	setStatus(tStatus newStatus);
	int		getFd() const;

private:
	std::string	_buff;
	tStatus		_status;
	int			_fd;

};


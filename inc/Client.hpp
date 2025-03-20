
#pragma once

#include "SharedFd.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
#define READSIZE	100

typedef enum eState {
	DFLT,
	READING,
	FINISHED_READING,
	WRITING,
	FINISHED_WRITING,
	FINISHED
} tState;

class Client {
public:
	Client(SharedFd fd);
	~Client();

	void	readFrom();
	void	writeTo();

	tState	getState() const;
	void	setState(tState newState);
	int		getFd() const;

private:
	std::string	_buff;
	tState		_state;
	int			_fd;

};


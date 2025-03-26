
#pragma once

#include "Config.hpp"
#include "SharedFd.hpp"
#include <functional>
#include <unistd.h>
#include <sys/epoll.h>
#define READSIZE	100

typedef enum eState {
	REQUEST,
	RESPONSE,
	CGI,
	DONE
} tState;

class Client {
public:
	Client(
		SharedFd clientFd,
		SharedFd serverFd,
		std::function<void(struct epoll_event)> addToEpoll_cb,
		std::function<const Config* const (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb
	);
	Client(const Client& other) = delete;
	Client& operator=(const Client& other) = delete;
	~Client();

	void		handle(struct epoll_event epollData);
	inline bool	isDone() const { return (_state == DONE); }

private:
	SharedFd		_clientSock;
	SharedFd		_serverSock;
	tState			_state;
	const Config	*_config;

	// callback to add a file descriptor to Webserv epoll instance
	std::function<void(struct epoll_event)> _addToEpoll_cb;
	// callback to set the specified config through Webserv method
	std::function<void(const std::string& serverName)> _setConfig_cb;
};


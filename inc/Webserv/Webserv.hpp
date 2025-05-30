#pragma once

#include <memory>
#include <unordered_map>
#include <csignal>
#include <netdb.h>

#include "../Config/Config.hpp"
#include "../HTTP/HTTPClient.hpp"
#include "Epoll.hpp"
#include "SharedFd.hpp"

// C libs

// DEFAULTS
# define DEFAULT_PORT 8080
# define DEFAULT_CLIENT_BODY_SIZE 1048576 // 1m or 1MB
# define DEFAULT_INDEX "index.html"
# define DEFAULT_AUTOINDEX false
# define DEFAULT_LOCATION "/"
# define DEFAULT_CONFIGFILE "configs/default.conf"
# define DEFAULT_MIME_TYPES "configs/default.conf"
# define MAX_CONFIG_FILE_SIZE 102400 // 100kb

class Webserv {
public:
	Webserv(const std::string& confPath);
	Webserv(const Webserv& other) = delete;
	Webserv& operator=(const Webserv& other) = delete;
	~Webserv();

	void	eventLoop();

private:
	std::unordered_map<SharedFd, std::vector<std::shared_ptr<Config>>>	_servers;
	std::unordered_map<SharedFd, HTTPClient> 			_clients;
	Epoll												_ep;

	void	_addClient(const SharedFd& clientSock, const SharedFd& servSock);
	void	_delClient(const SharedFd& fd);
	void	_delPipes(std::vector<int> to_delete);
	void	_handleServerReady(const struct epoll_event& ev);
	void	_handleClientReady(const struct epoll_event& ev);
};

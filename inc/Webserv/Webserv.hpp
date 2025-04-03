#pragma once

#include <unordered_map>

#include "../Config/ConfigParser.hpp"
#include "../Config/Config.hpp"
#include "../HTTP/HTTPClient.hpp"
#include "Epoll.hpp"
#include "SharedFd.hpp"
#include "Socket.hpp"
#include "sock.hpp"
#include "Colors.hpp"

// DEFAULTS
# define DEFAULT_PORT 8080
# define DEFAULT_CLIENT_BODY_SIZE 1048576 // 1m or 1MB
# define DEFAULT_INDEX "index.html"
# define DEFAULT_AUTOINDEX false
# define DEFAULT_LOCATION "/"
# define DEFAULT_CONFIGFILE "configs/default.conf"
# define DEFAULT_MIME_TYPES "configs/default.conf"

class HTTPClient;

class Webserv {
public:
	Webserv(const std::string& confPath);
	Webserv(const Webserv& other) = delete;
	Webserv& operator=(const Webserv& other) = delete;
	~Webserv();

	void	mainLoop();

private:
	std::unordered_map<SharedFd, std::vector<Config>>	_servers;
	std::unordered_map<SharedFd, HTTPClient> 			_clients;
	Epoll												_ep;
	std::vector<Config> 								_configs;

	void	_addClient(const SharedFd& clientSock, const SharedFd& servSock);
	void	_delClient(const SharedFd& fd);
};

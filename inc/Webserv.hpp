#pragma once

#include <unordered_map>
#include "Server.hpp"
#include "Client.hpp"
#include "Epoll.hpp"
#include "SharedFd.hpp"
#include "ConfigParser.hpp"
#include "Config.hpp"
#include <stdexcept>


class Webserv {
public:
	Webserv(const std::string& confPath);
	Webserv(const Webserv& other) = delete;
	Webserv& operator=(const Webserv& other) = delete;
	~Webserv();

	void	mainLoop();

private:
	std::unordered_map<SharedFd, std::vector<Server>>	 _servers;
	std::unordered_map<SharedFd, Client> 				_clients;
	Epoll											_ep;

	void	addClient(SharedFd& fd);
	void	handleClient(uint32_t events, SharedFd& fd);
	void	delClient(SharedFd& fd);
	const Server&	getServer(SharedFd& fd, const std::string& servName) const;
};

#pragma once

#include <unordered_map>
#include "Server.hpp"
#include "Client.hpp"
#include "Epoll.hpp"
#include <stdexcept>


class Webserv {
public:
	Webserv(const std::string& confPath);
	Webserv(const Webserv& other) = delete;
	Webserv& operator=(const Webserv& other) = delete;
	~Webserv();

	void	mainLoop();

private:
	std::unordered_map<int, std::vector<Server>>	 _servers;
	std::unordered_map<int, Client> 				_clients;
	Epoll											_ep;

	void	addClient(int fd);
	void	handleClient(uint32_t events, int fd);
	void	delClient(int fd);
	Server&	getServer(int fd, const std::string& servName);
};

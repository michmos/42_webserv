
#include "../inc/Webserv.hpp"

Webserv::Webserv(const std::string& confPath) {
	// ConfigParser	config(confPath);
	// std::unordered_set<int> sockets;
	//
	// for (auto config : config.getConfigs()) {
	// 	create Servers with config and Sockets
	//
	// }

}

Webserv::~Webserv() {
}

void	Webserv::addClient(int fd) {
	auto it = _clients.find(fd);
	if (it == _clients.end()) {
		throw std::runtime_error("addClient() trying to add existing client");
	}
	
	// TODO: listening socket needs to accept first
	_ep.add(fd, EPOLLIN | EPOLLOUT);
	_clients.emplace(fd, Client(fd));
}

void	Webserv::delClient(int fd) {
	auto it = _clients.find(fd);
	if (it != _clients.end()) {
		throw std::runtime_error("delClient() trying to del non-existant client");
	}

	_ep.del(fd);
	_clients.erase(it);
	// TODO: should I close fd here or is it closed somewhere else
	// TODO: lets close it in Client destructor
}

// Server&	Webserv::getServer(int fd, const std::string& servName) {
// 	auto it = _servers.find(fd);
// 	if (it == _servers.end()) {
// 		throw std::invalid_argument("getServer(): invalid fd");
// 	}
//
// 	for (auto server : it->second) {
// 		if (server.getName() == servName) {
// 			return (server);
// 		}
// 	}
// 	throw std::invalid_argument("getServer(): invalid server name");
// }

void	Webserv::handleClient(uint32_t events, int fd) {
	auto it = _clients.find(fd);
	if (it == _clients.end()) {
		throw std::invalid_argument("handleClient() called with unadded client");
	}

	if (events & EPOLLIN) {
		it->second.readFrom();
	} else if (events & EPOLLOUT) {
		it->second.writeTo();
	} else if (events & (EPOLLHUP | EPOLLERR)) {
		this->delClient(fd);
	}
}

void	Webserv::mainLoop() {
	while (true) {
		auto events = _ep.wait();
		for (auto ev : events) {
			// #ifdef DEBUG
			// std::cout << "|SERVER| client: " << ev.data.fd << " "
			// 	<< ((ev.events & EPOLLIN) ? "EPOLLIN " : " ")
			// 	<< ((ev.events & EPOLLOUT) ? "EPOLLOUT " : " ")
			// 	<< ((ev.events & (EPOLLHUP | EPOLLERR)) ? "EPOLLHUP | EPOLLERR" : "") << std::endl;
			// #endif

			if (_servers.find(ev.data.fd) != _servers.end()) {
				this->addClient();
			} else {
				this->handleClient(ev.events, ev.data.fd);
			}
		}
	}
}


#include "../inc/Webserv.hpp"

Webserv::Webserv(const std::string& confPath) {
	std::vector configs = ConfigParser("pathToConfig").getConfigs();
	std::unordered_map<std::string, SharedFd> sockets;

	for (auto& config : configs) {
		SharedFd	serverFd;

		std::string key = config.getHost() + std::to_string(config.getPort());
		const auto& it = sockets.find(key);
		if (it == sockets.end()) {
			// no fitting socket existing
			Socket	serverSock;
			serverSock.bind(inet_addr(config.getHost().c_str()), htons(config.getPort()));
			serverSock.listen(5);

			serverFd = serverSock.getFd();
			sockets[key] = serverFd;
		} else {
			// use existing socket
			serverFd = it->second;
		}

		_servers[serverFd].push_back(config);
	}
}

Webserv::~Webserv() {
}

void	Webserv::addClient(SharedFd& fd) {
	auto it = _clients.find(fd);
	if (it != _clients.end()) {
		throw std::runtime_error("addClient(): trying to add existing client");
	}
	
	_ep.add(fd.get(), EPOLLIN | EPOLLOUT);
	_clients.emplace(fd, Client(fd));
}

void	Webserv::handleClient(uint32_t events, SharedFd& fd) {
	auto it = _clients.find(fd);
	if (it == _clients.end()) {
		throw std::invalid_argument("handleClient(): called with unadded client");
	}

	if (events & EPOLLIN) {
		it->second.readFrom();
	} else if (events & EPOLLOUT) {
		it->second.writeTo();
	} else if (events & (EPOLLHUP | EPOLLERR)) {
		this->delClient(fd);
		return;
	}

	// TODO: assign server to client after parsing of request headers
	// - maybe also move this somewhere in if else tree above
	//
	// if (it->second.getState() == FINISHED_READING) {
	// 	it->second.setServer(this->getServer(it->second.getServerFd(), it->second.getServerName());
	// }
}

void	Webserv::delClient(SharedFd& fd) {
	auto it = _clients.find(fd);
	if (it == _clients.end()) {
		throw std::runtime_error("delClient(): trying to del non-existant client");
	}

	_ep.del(fd.get());
	_clients.erase(it);
}


const Server&	Webserv::getServer(SharedFd& fd, const std::string& servName) const {
	auto it = _servers.find(fd);
	if (it == _servers.end()) {
		throw std::invalid_argument("getServer(): invalid fd");
	}

	for (auto& server : it->second) {
		if (server.getName() == servName) {
			return (server);
		}
	}
	throw std::invalid_argument("getServer(): invalid server name");
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

			SharedFd fd = ev.data.fd;
			const auto& it = _servers.find(fd);
			if (it != _servers.end()) {
				// listening socket ready
				SharedFd newClient = Socket::sAccept(fd);
				this->addClient(newClient);
			} else {
				this->handleClient(ev.events, fd);
			}
		}
	}
}

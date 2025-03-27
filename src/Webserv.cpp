
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
			serverFd = sock::create();
			sock::bind(serverFd.get(), inet_addr(config.getHost().c_str()), htons(config.getPort()));
			sock::listen(serverFd.get(), 5);

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

void	Webserv::addClient(const SharedFd& clientSock, const SharedFd& servSock) {
	if (_clients.find(clientSock) != _clients.end()) {
		throw std::runtime_error("addClient(): trying to add existing client");
	}
	
	_ep.add(clientSock.get(), EPOLLIN | EPOLLOUT);
	_clients.emplace(
		clientSock, 
		Client(
			clientSock,
			servSock,
			// function to add an epoll event to epoll instance - used for callback
			std::move([this](struct epoll_event ev) {
				_ep.add(ev.data.fd, ev.events);
			}),
			// function to get ptr to config - used for callback
			std::move([this](const SharedFd& servSock, const std::string& servName) {
				return(this->getConfig(servSock, servName));
			})
		)
	);
}

void	Webserv::delClient(const SharedFd& clientSock) {
	const auto& it = _clients.find(clientSock);
	if (it == _clients.end()) {
		throw std::runtime_error("delClient(): trying to del non-existant client");
	}

	_ep.del(clientSock.get());
	_clients.erase(it);
}


// TODO: could be moved in client contructor call as lambda
const Config* const	Webserv::getConfig(const SharedFd& socket, const std::string& servName) const {
	auto it = _servers.find(socket);
	if (it == _servers.end()) {
		throw std::invalid_argument("getServer(): invalid fd");
	}

	for (auto& server : it->second) {
		if (server.getServerName() == servName) {
			return (&server);
		}
	}
	return(&(it->second[0]));
}

void	Webserv::mainLoop() {
	while (true) {
		const auto& events = _ep.wait();
		for (const auto& ev : events) {
			// #ifdef DEBUG
			// std::cout << "|SERVER| client: " << ev.data.fd << " "
			// 	<< ((ev.events & EPOLLIN) ? "EPOLLIN " : " ")
			// 	<< ((ev.events & EPOLLOUT) ? "EPOLLOUT " : " ")
			// 	<< ((ev.events & (EPOLLHUP | EPOLLERR)) ? "EPOLLHUP | EPOLLERR" : "") << std::endl;
			// #endif

			SharedFd fd = ev.data.fd;
			const auto& it = _servers.find(fd);
			if (it != _servers.end()) {
				// is listening socket
				SharedFd clientSock = sock::accept(it->first.get());
				this->addClient(clientSock, it->first);
			} else {
				// is existing client
				_clients[it->first].handle(ev);
			}
		}
	}
}

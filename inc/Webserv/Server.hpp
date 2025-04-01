#pragma once

#include <string>

#include "../Config/Config.hpp"
#include "Socket.hpp"

class Server {
public:
	Server(const Socket& serverSock, const Config& _conf, const std::string& name);
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	~Server();

	inline const Socket& getSocket() const { return (_sock); }
	inline const std::string& getName() const { return (_name); }
	inline const Config& getConfig() const { return (_config); }

private:
	Socket			_sock;
	std::string		_name;
	Config			_config;
};
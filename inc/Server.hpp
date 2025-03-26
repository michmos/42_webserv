#pragma once

#include "Config.hpp"
#include "SharedFd.hpp"
#include <string>

class Server {
public:
	Server(SharedFd lstngSock, const std::string& name, const Config& conf);
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	~Server();

	inline const SharedFd& getFd() const { return (_lstngSock); }
	inline const std::string& getName() const { return (_name); }
	inline const Config& getConfig() const { return (_config); }

private:
	SharedFd		_lstngSock;
	std::string		_name;
	Config			_config;
};


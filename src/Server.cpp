
#include "../inc/Server.hpp"

Server::Server(SharedFd lstngSock, const std::string& name, const Config& conf)
	: _lstngSock(lstngSock), _name(name), _config(conf) {};

Server::~Server() {
}


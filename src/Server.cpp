#include "../inc/Server.hpp"

#include "../inc/Server.hpp"

Server::Server(const Socket& serverSock, const Config& conf, const std::string& name)
	: _sock(serverSock), _name(name), _config(conf) {
}

Server::~Server() {
}
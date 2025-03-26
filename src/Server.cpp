#include "../inc/Server.hpp"

#include "../inc/Server.hpp"

Server::Server(const Socket& serverSock, const Config& conf, const std::string& name)
	: _sock(serverSock), _config(conf), _name(name) {
}

Server::~Server() {
}
#include "../inc/Config.hpp"

Config::Config() {
}

Config::~Config() {
}

Config::Config(const Config &toCopy) {
	*this = toCopy;
}

Config& Config::operator=(const Config &other) {
	if (this != &other) {
	}
	return *this;
}

std::vector<ServerConfig> Config::getServers() const {
	return this->_servers;
}

void Config::setServer(ServerConfig server) {
	this->_servers.push_back(server);
}

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

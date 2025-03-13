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

int Config::setDirective(const std::string key, std::vector<std::string> values) {
	if (_directives.find(key) != _directives.end())
		return (-1);
	for (std::string str: values) {
		_directives[key].push_back(str);
	}
	return (0);
}

int Config::setLocation(const std::string key, Location loc) {
	if (_locations.find(key) != _locations.end())
		return (-1);
	_locations[key] = loc;
	return (0);
}



// struct Location {
// 	bool														strict_match;
// 	std::unordered_map<std::string, std::vector<std::string>>	directives;
// };

// class Config {
// 	private:
// 		std::unordered_map<std::string, std::vector<std::string>>	_directives;
// 		std::unordered_map<std::string, Location>					_locations;

void Config::printConfig() {
	std::cout << BOLD << "\n CONFIG PRINT - DIRECTIVES:" << RESET << std::endl;
	for (auto it = _directives.begin(); it != _directives.end(); it++) {
		std::cout << it->first << " : ";
		for (const std::string &str : it->second)
			std::cout << str << " ";
		std::cout << "\n";
	}
	std::cout << BOLD << " LOCATIONS:" << RESET << std::endl;
	for (auto it_loc = _locations.begin(); it_loc != _locations.end(); it_loc++) {
		std::cout << it_loc->first << "\n";
		for (auto it = it_loc->second.directives.begin(); it != it_loc->second.directives.end(); it++) {
			std::cout << "\t" << it->first << " : ";
			for (const std::string &str : it->second)
				std::cout << str << " ";
			std::cout << "\n";
		}
	}
}

// GETTER
std::unordered_map<std::string, Location>	Config::getLocations() {
	return (this->_locations);
}
std::unordered_map<std::string, std::vector<std::string>> Config::getDirectives() {
	return (this->_directives);
}

int	Config::getPort() {
	int port = 0;
	std::string strPort = (*(this->_directives.find("listen"))).second.front();
	// try {
	port = stoi(strPort);
	// } catch (std::exception &e) {
	// 	throw Config::ConfigException(e.what());
	// }
	// if (port < 0 || port > 65535)
	// 	throw Config::ConfigException("Port out of range!");
	return (port);
}

std::string	Config::getHost() {
	return ((*(this->_directives.find("host"))).second.front());
}

std::string	Config::getServerName() {
	return ((*(this->_directives.find("server_name"))).second.front());
}

std::uint64_t	Config::getClientBodySize() {
	std::uint64_t	size;
	std::string strSize = (*(this->_directives.find("listen"))).second.front();
	size = stoi(strSize);
	if (tolower(strSize.back()) == 'k')
		size *= 1024;
	else if (tolower(strSize.back()) == 'm')
		size *= (1024*1024);
	else if (tolower(strSize.back()) == 'g')
		size *= (1024*1024*1024);
	return (size);
}

// // return 301 http://example.com/newpage;
// std::vector<std::string>	Config::getRedirect(const std::string locKey) {
// 	if (locKey == "/")
// 		return ((*(this->_directives.find("return"))).second);
// }




// root /tmp/www;
std::vector<std::string>	Config::getRoot(const std::string locKey) {
	if (locKey != "/") {
		auto it = this->_locations.find(locKey);
		if (it != this->_locations.end()) {
			Location loc = it->second;
			auto it_root = loc.directives.find("root");
			if (it_root != loc.directives.end()) {

			}
		}
	}
	return ((*(this->_directives.find("root"))).second);
}

// allow_methods  DELETE POST GET;
std::vector<std::string>	Config::getMothods(const std::string locKey) {
	return ((*(this->_directives.find("return"))).second);
}

// index index.html index.php;
std::vector<std::string>	Config::getIndex(const std::string locKey) {
	return ((*(this->_directives.find("return"))).second);
}


// bool	Config::getLocAutoindex(const std::string locKey);	// autoindex on;
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
		this->_directives = other._directives;
		this->_locations = other._locations;
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

void Config::printConfig() {
	std::cout << BOLD << BG_LIGHT_GRAY << BLACK << "\n CONFIG PRINT - DIRECTIVES:" << RESET << std::endl;
	for (auto it = _directives.begin(); it != _directives.end(); it++) {
		std::cout << "\t" << it->first << " : ";
		for (const std::string &str : it->second)
			std::cout << str << " ";
		std::cout << "\n";
	}
	std::cout << BOLD << BG_LIGHT_GRAY << BLACK << "\n LOCATIONS:" << RESET << std::endl;
	for (const auto &pair: _locations)
	{
		std::cout << BOLD << pair.first << ":\n" << RESET;
		std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(pair.first);
		for (const auto &str: dirMap)
		{
			std::cout << "\t" << str.first << " : ";
			for (const std::string &str2: str.second)
				std::cout << str2 << " ";
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}

// GETTER
std::unordered_map<std::string, std::vector<std::string>> Config::getDirectives() {
	return (this->_directives);
}
std::unordered_map<std::string, Location>	Config::getLocations() {
	return (this->_locations);
}

int	Config::getPort() {
	int port;
	std::string strPort;
	auto it = this->_directives.find("listen");
	if (it != this->_directives.end()) {
		if (it->second.size() > 0) {
			strPort = it->second[0];
		}
	} else {
		strPort = "80";
	}
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
	// return ((*(this->_directives.find("host"))).second.front());
	auto it = this->_directives.find("host");
	if (it != this->_directives.end()) {
		if (it->second.size() > 0) {
			return (it->second[0]);
		}
	}
	return ("localhost");
}

std::string	Config::getServerName() {
	// return ((*(this->_directives.find("server_name"))).second.front());
	auto it = this->_directives.find("server_name");
	if (it != this->_directives.end()) {
		if (it->second.size() > 0) {
			return (it->second[0]);
		}
	}
	return ("");
}

// client_max_body_size 10M;
std::uint64_t	Config::getClientBodySize(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(locKey);
	auto it = dirMap.find("client_max_body_size");
	std::string strSize;
	if (it != dirMap.end()) {
		if (it->second.size() > 0) {
			strSize = it->second[0];
		}
	} else {
		strSize = "1m";
	}
	std::uint64_t	size;
	size = stoi(strSize);
	if (tolower(strSize.back()) == 'k')
		size *= 1024;
	else if (tolower(strSize.back()) == 'm')
		size *= (1024*1024);
	else if (tolower(strSize.back()) == 'g')
		size *= (1024*1024*1024);
	return (size);
}

// return 301 http://example.com/newpage;
std::vector<std::string>	Config::getRedirect(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(locKey);
	auto it = dirMap.find("return");
	if (it != dirMap.end()) {
		if (it->second.size() > 0) {
			return it->second;
		}
	}
	return (std::vector<std::string>());
}

// root /tmp/www;
std::vector<std::string>	Config::getRoot(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(locKey);
	auto it = dirMap.find("index");
	if (it != dirMap.end()) {
		if (it->second.size() > 0) {
			return it->second;
		}
	}
	return (std::vector<std::string>());
}

// // allow_methods  DELETE POST GET;
std::vector<std::string>	Config::getMethods(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(locKey);
	auto it = dirMap.find("allow_methods");
	if (it != dirMap.end()) {
		if (it->second.size() > 0) {
			return it->second;
		}
	}
	return (std::vector<std::string>());
}

// index index.html index.php;
std::vector<std::string>	Config::getIndex(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(locKey);
	auto it = dirMap.find("index");
	if (it != dirMap.end()) {
		if (it->second.size() > 0) {
			return it->second;
		}
	}
	return (std::vector<std::string>());
}

// autoindex on;
bool	Config::getAutoindex(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> dirMap = this->getLocDirectives(locKey);
	auto it = dirMap.find("autoindex");
	if (it != dirMap.end()) {
		if (it->second.size() > 0) {
			if (it->second[0] == "on")
				return (true);
			else if (it->second[0] == "off")
				return (false);
		}
	}
	return (false);
}

std::unordered_map<std::string, std::vector<std::string>> Config::getLocDirectives(const std::string locKey) {
	std::unordered_map<std::string, std::vector<std::string>> locMap = this->_directives;
	size_t pos = 0;
	std::string key = "/";
	while (pos < locKey.size()) {
		auto it = this->_locations.find(key);
		if (it != this->_locations.end()) {
			Location loc = it->second;
			if (loc.strict_match == false || pos + 1 == locKey.size()) {
				for (auto it_loc = loc.directives.begin(); it_loc != loc.directives.end(); it_loc++) {
					locMap[it_loc->first] = it_loc->second;
				}
			}
		}
		pos = locKey.find('/', pos + 1);
		key = locKey.substr(0, pos + 1);
	}
	// std::cout << BOLD << "\n\n DIR RETURN LOC:" << RESET << std::endl;
	// for (const auto &str: locMap)
	// {
	// 	std::cout << str.first << " : ";
	// 	for (const std::string &str2: str.second)
	// 		std::cout << str2 << " ";
	// 	std::cout << "\n";
	// }
	// std::cout << "\n";
	return (locMap);
}
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
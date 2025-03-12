#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"


// LOOK INTO: location in a location
// map can be unordered_map
struct Location {
	bool														strict_match;
	std::unordered_map<std::string, std::vector<std::string>>	directives;
};

class Config {
	private:
		std::unordered_map<std::string, std::vector<std::string>>	_directives;
		std::unordered_map<std::string, Location>					_locations;
	public:
		Config();
		~Config();
		Config(const Config &toCopy);
		Config& operator=(const Config &other);

		// GETTERS

		// SETTERS
		int	setLocation(std::string key, Location loc);
		int	setDirective(std::string key, std::vector<std::string> values);

		// UTILS
		void	printConfig();

		// EXCEPTIONS
		class ConfigException : public std::exception {
			private:
				std::string message;
			public:
				ConfigException(const std::string &msg) : message(msg) {}
				virtual const char* what() const throw() {
					return message.c_str();
				}
		};
};

#endif // SERVERCONFIG_HPP

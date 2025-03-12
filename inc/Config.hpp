#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"


// LOOK INTO: location in a location
// map can be unordered_map
struct Location {
	std::map<std::string, std::string> directives;
};

class Config {
	private:
		std::map<std::string, std::string> directives;
		std::map<std::string, Location> locations;
	public:
		Config();
		~Config();
		Config(const Config &toCopy);
		Config& operator=(const Config &other);

		// GETTERS

		// SETTERS

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

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"
// #include "Location.hpp"



// LOOK INTO: location in a location
// map can be unordered_map
struct Location {
    std::map<std::string, std::string> directives;
};

struct ServerConfig {
    std::map<std::string, std::string> directives;
    std::map<std::string, Location> locations;
};

class Config {
	private:
		std::vector<ServerConfig>		_servers;
	public:
		Config();
		~Config();
		Config(const Config &toCopy);
		Config& operator=(const Config &other);

		// GETTERS
		std::vector<ServerConfig> getServers() const;

		// SETTERS
		void setServer(ServerConfig servers);

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

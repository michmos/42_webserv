#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Webserv.hpp"
#include "Location.hpp"

class ServerConfig {
	private:
		std::vector<std::string>		_host;
		std::vector<int>				_port;
		std::vector<std::string>		_server_name;
		std::vector<std::string>		_root;
		std::vector<std::string>		_index;
		std::vector<std::string>		_error_page;
		// std::vector<std::string>		_client_max_body_size;
		// std::vector<std::string>		_autoindex;
		// std::vector<std::string>		_auth_basic;
		// std::vector<std::string>		_auth_basic_user_file;
		// std::vector<std::string>		_location;

	public:
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &toCopy);
		ServerConfig& operator=(const ServerConfig &other);

		// GETTERS
		std::vector<Location> getLocations() const;

		// SETTERS
		// void setLocations(std::vector<Location> locations);

		// EXCEPTIONS
        class ServerConfigException : public std::exception {
            private:
                std::string message;
            public:
                ServerConfigException(const std::string &msg) : message(msg) {}
                virtual const char* what() const throw() {
                    return message.c_str();
                }
        };
};

#endif // SERVERCONFIG_HPP

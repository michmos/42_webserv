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

		// UTILS
		std::map<std::string, std::vector<std::string>> getPathRange(const std::string locKey);
	public:
		Config();
		~Config();
		Config(const Config &toCopy);
		Config& operator=(const Config &other);

		// SETTERS
		int	setLocation(std::string key, Location loc);
		int	setDirective(std::string key, std::vector<std::string> values);

		// GETTERS
		std::unordered_map<std::string, std::vector<std::string>>	getDirectives();
		std::unordered_map<std::string, Location>					getLocations();

		int				getPort();
		std::string		getHost();
		std::string		getServerName();
		std::uint64_t	getClientBodySize();

		std::vector<std::string>	getRedirect(const std::string locKey);	// return 301 http://example.com/newpage;
		std::vector<std::string>	getRoot(const std::string locKey);		// root /tmp/www;
		std::vector<std::string>	getMothods(const std::string locKey);	// allow_methods  DELETE POST GET;
		std::vector<std::string>	getIndex(const std::string locKey);		// index index.html index.php;

		bool						getLocAutoindex(const std::string locKey);	// autoindex on;





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

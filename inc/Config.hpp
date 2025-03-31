#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// LOOK INTO:
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
	public:
		Config();
		~Config();
		Config(const Config &toCopy);
		Config& operator=(const Config &other);

		// SETTERS
		int	setLocation(std::string key, Location loc);
		int	setDirective(std::string key, std::vector<std::string> values);

		// GETTERS
		std::unordered_map<std::string, std::vector<std::string>>	getDirectives() const;
		std::unordered_map<std::string, Location>					getLocations() const;

		std::unordered_map<std::string, std::vector<std::string>> getLocDirectives(const std::string locKey) const;

		//		need for WebServer Class
		int				getPort();
		std::string		getHost();
		std::string		getServerName();

		//		need for Client Class
		bool						getAutoindex(const std::string locKey) const;		// autoindex on;
		std::uint64_t				getClientBodySize(const std::string locKey) const;	// client_max_body_size 10M;
		std::vector<std::string>	getRedirect(const std::string locKey) const;		// return 301 http://example.com/newpage;
		std::vector<std::string>	getRoot(const std::string locKey) const;			// root /tmp/www;
		std::vector<std::string>	getMethods(const std::string locKey) const;			// allow_methods  DELETE POST GET;
		std::vector<std::string>	getIndex(const std::string locKey) const;			// index index.html index.php;

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
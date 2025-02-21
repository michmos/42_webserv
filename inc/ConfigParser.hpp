#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"
#include "ServerConfig.hpp"

class ConfigParser {
	private:
		std::string					_filepath;
		std::vector<std::string>	_lines;
		std::vector<ServerConfig>			_config;

		// readConfigToLines UTILS
		std::ifstream openConfigFile(std::string& filepath);
		void readFile(std::ifstream& config_file);
		void cleanWhitespace(std::string& line);
		void cleanComments(std::string& line);
		bool cleanEmptyLines(std::string& line);
		void closeFile(std::ifstream& file);
	public:
		ConfigParser();
		explicit ConfigParser(const std::string& filepath);
		~ConfigParser();
		ConfigParser(const ConfigParser &toCopy);
		ConfigParser& operator=(const ConfigParser &other);

		// PARSING
		void readConfigToLines();
		void parseConfig();

		// GETTERS
		std::vector<ServerConfig> getConfig() const;
		// std::vector<std::string> getLines() const;

		// SETTERS
		void setServers(std::vector<ServerConfig> servers);
		void setLines(std::vector<std::string> lines);

		// PRINT
		// void printConfig();
		void printLines();

		// EXCEPTIONS
        class ConfigParserException : public std::exception {
            private:
                std::string message;
            public:
                ConfigParserException(const std::string &msg) : message(msg) {}
                virtual const char* what() const throw() {
                    return message.c_str();
                }
        };
};

#endif // CONFIGPARSER_HPP

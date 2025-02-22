#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Webserv.hpp"
#include "Config.hpp"

class ConfigParser {
	private:
		std::string					_filepath;
		std::vector<std::string>	_lines;
		Config						_config;

		// readConfigToLines UTILS
		// std::ifstream openConfigFile(std::string& filepath);
		void readFile(std::ifstream& config_file);
		// void closeFile(std::ifstream& file);
	public:
		ConfigParser();
		explicit ConfigParser(const std::string& filepath);
		~ConfigParser();
		ConfigParser(const ConfigParser &toCopy);
		ConfigParser& operator=(const ConfigParser &other);

		// PARSING
		void readConfigToLines();
		void parseConfigLines();

		// GETTERS
		Config getConfig() const;

		// UTILS
		std::string cleanWhitespace(const std::string &line);
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

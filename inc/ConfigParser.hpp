#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP
#include "Webserv.hpp"
#include "Config.hpp"

// ENUM IDEAS
// Whitespace: Spaces, tabs, newlines (used for token separation but not meaningful).
// Comments: # starts a comment until the end of the line.
// Blocks: { and } to define hierarchical structures.
// Semicolon: ; to end statements.
// Strings: "value" or 'value' and index.html
// Numbers: Integers and floating-point numbers, often used for ports and timeouts.
// Operators: =, ~, !~, ^~, etc., used in regex and conditionals.
// Paths: /path/to/resource, used for file system paths.
// URLs:  http://example.com, used for proxy_pass and redirects.
// Variables: $variable_name, used for dynamic content.

enum tokenType {
	INIT,
	WHITE_SPACE,
	SERVER,
	BLOCK_OPEN,
	BLOCK_CLOSE,
	SEMICOLON,
	STRING,
	NUMBER,
	OPERATOR,
	PATH,
	URL,
	// VARIABLE, // curently not implemented
	EOF_TOKEN
};

// TOKENS FOR LEXAR
struct token {
	std::string::iterator		itStart;
	std::string::iterator		itEnd;
	std::string		value;
	tokenType		type;
};

class ConfigParser {
	private:
		std::string					_filepath;
		std::string					_input;
		std::vector<Config>			_config;
		std::vector<token>			_tokens;

		token getNextToken(token &lastToken, const std::regex &url, const std::regex &path, const std::regex &op);

		// readConfigToLines UTILS
		// std::ifstream openConfigFile(std::string& filepath);
		void readFile(std::ifstream& config_file);
		// void closeFile(std::ifstream& file);
		std::vector<std::string>::iterator parseServerLines(std::vector<std::string>::iterator it, Config &nextServerConfig);
	public:
		ConfigParser();
		explicit ConfigParser(const std::string& filepath);
		~ConfigParser();
		ConfigParser(const ConfigParser &toCopy);
		ConfigParser& operator=(const ConfigParser &other);
		// PARSING
		void readConfigToInput();
		void parseInputToTokens();
		
		// GETTERS
		Config getConfig() const;
		// UTILS
		std::string cleanWhitespace(const std::string &line);
		void printInput();
		void printTokens();
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

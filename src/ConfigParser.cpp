#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser() {
}

ConfigParser::ConfigParser(const std::string& filepath) {
	this->_filepath = filepath;
}

ConfigParser::~ConfigParser() {
}

ConfigParser::ConfigParser(const ConfigParser &toCopy) {
	*this = toCopy;
}

ConfigParser& ConfigParser::operator=(const ConfigParser &other) {
	if (this != &other) {
		this->_filepath = other._filepath;
		this->_input = other._input;
	}
	return *this;
}

std::ifstream openConfigFile(std::string& filepath) {
	if (filepath.empty() || filepath.length() == 0) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: config filepath is empty");
	}
	std::ifstream config_file;
	config_file.open(filepath.c_str());
	if (!config_file || !config_file.is_open()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: could not open config file");
	}
	return (config_file);
}

void cleanComments(std::string &line) {
	size_t comment_pos = line.find("#");
	if (comment_pos != std::string::npos) {
		line.erase(comment_pos);
	}
}

void ConfigParser::readFile(std::ifstream& config_file) {
	std::vector<std::string> lines;
	std::string line;
	while (config_file.peek() != EOF) {
		std::getline(config_file, line);
		cleanComments(line);
		lines.push_back(line);
		lines.push_back("\n");
	}
	this->_input = std::accumulate(lines.begin(), lines.end(), std::string(""));
}

void closeFile(std::ifstream& file) {
	file.close();
	if (file.fail()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: could not close config file.");
	} else if (file.bad()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: Critical I/O error! Bad config file");
	}
}

void ConfigParser::readConfigToInput() {
	try {
		std::ifstream config_file = openConfigFile(this->_filepath);
		this->readFile(config_file);
		closeFile(config_file);
	} catch (std::exception &e) {
		throw ConfigParser::ConfigParserException(e.what());
	}
}

// std::string ConfigParser::cleanWhitespace(const std::string &line) {
// 	std::string cleanedLine = line;
// 	cleanedLine.erase(0, cleanedLine.find_first_not_of(" \t"));
// 	cleanedLine.erase(cleanedLine.find_last_not_of(" \t") + 1);
// 	return cleanedLine;
// }

void ConfigParser::printInput() {
	std::cout << this->_input << std::endl;
}


// Get the server block as a vector of strings
// std::vector<std::string> extractServerBlock(std::vector<std::string>& lines) {
// 	std::vector<std::string> extractedBlock;
// 	bool capturing = false;
// 	int braceCount = 0;
// 	auto it = lines.begin();

// 	while (it != lines.end()) {
// 		if (!capturing) {
// 			if (it->find("server") != std::string::npos && it->find("{") != std::string::npos) {
// 				capturing = true;
// 				braceCount = 1;
// 				extractedBlock.push_back(*it);
// 				it = lines.erase(it);
// 				continue;
// 			}
// 		} else {
// 			extractedBlock.push_back(*it);
// 			if (it->find("{") != std::string::npos) {
// 				braceCount++;
// 			}
// 			if (it->find("}") != std::string::npos) {
// 				braceCount--;
// 				if (braceCount == 0) {
// 					it = lines.erase(it);
// 					break;
// 				}
// 			}
// 			it = lines.erase(it);
// 			continue;
// 		}
// 		++it;
// 	}
// 	return extractedBlock;
// }
// void ConfigParser::parseConfigLines() {
// 	std::vector<std::string> serverBlock;
// 	serverBlock = extractServerBlock(this->_lines);
// 	while (!serverBlock.empty())
// 	{
// 		for (std::vector<std::string>::iterator it = serverBlock.begin(); it != serverBlock.end(); it++) {
// 			std::cout << GREEN << *it << RESET << std::endl;
// 		}
// 		std::cout << BOLD << CYAN << "Server block END \n\n" << RESET << std::endl;
// 		serverBlock = extractServerBlock(this->_lines);
// 	}
// }

// std::vector<std::string>::iterator ConfigParser::parseServerLines(std::vector<std::string>::iterator it, Config &nextServerConfig) {
// 	std::string line;
// 	for (; it != this->_lines.end(); it++) {
// 		line = *it;
// 		if (line.find("{") != std::string::npos) {
// 			std::cout << GREEN << "\"{\"-INDEX: " << std::distance(this->_lines.begin(), it) << RESET << std::endl;
// 			break;
// 		}
// 	}
// 	if (it == this->_lines.end()) {
// 		throw ConfigParser::ConfigParserException("ERROR PARSING: no opening brace found for server block started at line:" + (1 + std::distance(this->_lines.begin(), it)));
// 	}
// 	std::vector<std::string>::iterator itEnd = it;
// 	for (; itEnd != this->_lines.end(); itEnd++) {
// 		line = *itEnd;
// 		if (line.find("}") != std::string::npos) {
// 			break;
// 		}
// 	}
// 	return it;
// }

// void ConfigParser::parseConfigLines() {
// 	for (std::vector<std::string>::iterator it = this->_lines.begin(); it != this->_lines.end(); it++) {
// 		Config nextServerConfig;
// 		if (it->find("server ") != std::string::npos) {
// 			std::cout << GREEN << "\"Server\"-INDEX: " << std::distance(this->_lines.begin(), it) << RESET << std::endl;
// 			// it = parseServerLines(it, nextServerConfig);
// 			// checkServerConfig(nextServerConfig);
// 			this->_config.push_back(nextServerConfig);
// 		}
// 	}
// 	if (_config.empty()) {
// 		throw ConfigParser::ConfigParserException("ERROR PARSING: no server blocks found in config file");
// 	}
// }


void ConfigParser::printTokens() {
	for (std::vector<token>::iterator it = _tokens.begin(); it != _tokens.end(); it++) {
		switch (it->type) {
		case SERVER:
			std::cout << BG_BRIGHT_BLUE << it->value << RESET;
			break;
		case BLOCK_OPEN:
			std::cout << BG_BRIGHT_CYAN << it->value << RESET;
			break;
		case BLOCK_CLOSE:
			std::cout << BG_BRIGHT_GREEN << it->value << RESET;
			break;
		case SEMICOLON:
			std::cout << BG_BRIGHT_YELLOW << it->value << RESET;
			break;
		case STRING:
			std::cout << BG_BRIGHT_MAGENTA << it->value << RESET;
			break;
		case NUMBER:
			std::cout << BG_PINK << it->value << RESET;
			break;
		case WHITE_SPACE:
			if (it->value == "\n")
				std::cout << it->value;
			else
				std::cout << BG_BRIGHT_WHITE << it->value << RESET;
			break;
		case URL:
			std::cout << BG_CYAN << it->value << RESET;
			break;
		case PATH:
			std::cout << BG_GREEN << it->value << RESET;
			break;
		case OPERATOR:
			std::cout << BG_PURPLE << it->value << RESET;
			break;
		case EOF_TOKEN:
			std::cout << BG_RED << it->value << RESET;
			break;
		default:
			std::cout << BG_RED << it->value << RESET;
			break;
		}
	}
}

token ConfigParser::getNextToken(token &lastToken, const std::regex &url_regex, const std::regex &path_regex, const std::regex &op_regex) {
	token newToken;
	newToken.itStart = lastToken.itEnd;
	newToken.itEnd = std::find_if(newToken.itStart, this->_input.end(), [](char c) {
		return (c == ' ' || c == '\n' || c == '\t' || c == '{' || c == '}' || c == ';');
	});
	if (newToken.itStart == newToken.itEnd) {
		newToken.itEnd++;
	}
	newToken.value = std::string(newToken.itStart, newToken.itEnd);
	if (newToken.itEnd == this->_input.end()) {
		newToken.type = EOF_TOKEN;
	} else if (newToken.value == "server") {
		newToken.type = SERVER;
	} else if (newToken.value == "{") {
		newToken.type = BLOCK_OPEN;
	} else if (newToken.value == "}") {
		newToken.type = BLOCK_CLOSE;
	} else if (newToken.value == ";") {
		newToken.type = SEMICOLON;
	} else if (newToken.value == " " || newToken.value == "\t" || newToken.value == "\n") {
		newToken.type = WHITE_SPACE;
	} else if (!newToken.value.empty() && std::all_of(newToken.value.begin(), newToken.value.end(), ::isdigit)) {
		newToken.type = NUMBER;
	} else if (std::regex_match(newToken.value, op_regex)) {
		newToken.type = OPERATOR;
	} else if (std::regex_match(newToken.value, url_regex)) {
		newToken.type = URL;
	} else if (std::regex_match(newToken.value, path_regex)) {
		newToken.type = PATH;
	} else {
		newToken.type = STRING;
	}
	return newToken;
}

void ConfigParser::parseInputToTokens() {
	token newToken;
	static const std::regex url_regex(R"(https?:\/\/[a-zA-Z0-9\-_\.\/]+)");
	static const std::regex path_regex(R"(^(/[a-zA-Z0-9._~!$&'()*+,;=:@-]*)*$)");
	static const std::regex op_regex(R"([=~!^][=~!^|]*)");
	newToken.type = INIT;
	newToken.itEnd = this->_input.begin();
	while (newToken.type != EOF_TOKEN) {
		newToken = getNextToken(newToken, url_regex, path_regex, op_regex);
		this->_tokens.push_back(newToken);
	}
}


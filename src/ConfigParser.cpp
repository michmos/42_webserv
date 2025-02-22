#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser() {
	std::cout << BOLD << CYAN << "ConfigParser default constructor" << RESET << std::endl;
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
		this->_lines = other._lines;
		// this->_servers = other._servers;
		// this->_locations = other._locations;
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
	std::string line;
	while (config_file.peek() != EOF) {
		std::getline(config_file, line);
		cleanComments(line);
		this->_lines.push_back(line);
	}
}

void closeFile(std::ifstream& file) {
	file.close();
	if (file.fail()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: could not close config file.");
	} else if (file.bad()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: Critical I/O error! Bad config file");
	}
}

void ConfigParser::readConfigToLines() {
	try {
		std::ifstream config_file = openConfigFile(this->_filepath);
		this->readFile(config_file);
		closeFile(config_file);
	} catch (std::exception &e) {
		throw ConfigParser::ConfigParserException(e.what());
	}
}

std::string ConfigParser::cleanWhitespace(const std::string &line) {
	std::string cleanedLine = line;
	cleanedLine.erase(0, cleanedLine.find_first_not_of(" \t"));
	cleanedLine.erase(cleanedLine.find_last_not_of(" \t") + 1);
	return cleanedLine;
}

void ConfigParser::printLines() {
	for (std::vector<std::string>::iterator it = this->_lines.begin(); it != this->_lines.end(); it++) {
		std::cout << *it << std::endl;
	}
}

Config ConfigParser::getConfig() const {
	return this->_config;
}

std::vector<std::string> extractServerBlock(std::vector<std::string>& lines) {
	std::vector<std::string> extractedBlock;
	bool capturing = false;
	int braceCount = 0;
	auto it = lines.begin();

	while (it != lines.end()) {
		if (!capturing) {
			if (it->find("server") != std::string::npos && it->find("{") != std::string::npos) {
				capturing = true;
				braceCount = 1;
				extractedBlock.push_back(*it);
				it = lines.erase(it);
				continue;
			}
		} else {
			extractedBlock.push_back(*it);
			if (it->find("{") != std::string::npos) {
				braceCount++;
			}
			if (it->find("}") != std::string::npos) {
				braceCount--;
				if (braceCount == 0) {
					it = lines.erase(it);
					break;
				}
			}
			it = lines.erase(it);
			continue;
		}
		++it;
	}

	return extractedBlock;
}


void ConfigParser::parseConfigLines() {
	std::vector<std::string> serverBlock;
	serverBlock = extractServerBlock(this->_lines);
	while (!serverBlock.empty())
	{
		for (std::vector<std::string>::iterator it = serverBlock.begin(); it != serverBlock.end(); it++) {
			std::cout << GREEN << *it << RESET << std::endl;
		}
		std::cout << BOLD << CYAN << "Server block END \n\n" << RESET << std::endl;
		serverBlock = extractServerBlock(this->_lines);
	}


}

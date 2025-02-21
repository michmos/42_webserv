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

void ConfigParser::readConfigToLines() {
	try {
		std::ifstream config_file = openConfigFile(this->_filepath);
		this->readFile(config_file);
		this->closeFile(config_file);
	} catch (std::exception &e) {
		throw ConfigParser::ConfigParserException(e.what());
	}
}

std::ifstream ConfigParser::openConfigFile(std::string& filepath) {
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

void ConfigParser::readFile(std::ifstream& config_file) {
	std::string line;
	while (config_file.peek() != EOF) {
		std::getline(config_file, line);
		this->cleanComments(line);
		this->cleanWhitespace(line);
		if (this->cleanEmptyLines(line))
			continue;
		this->_lines.push_back(line);
	}
}

// UTILS FUNCTION TO CLEAN CONFIG FILE
void ConfigParser::cleanComments(std::string &line) {
	size_t comment_pos = line.find("#");
	if (comment_pos != std::string::npos) {
		line.erase(comment_pos);
	}
}

// UTILS FUNCTION TO CLEAN CONFIG FILE
void ConfigParser::cleanWhitespace(std::string &line) {
	// trim whitespace at beginning and end of line
	line.erase(0, line.find_first_not_of(" \t"));
	line.erase(line.find_last_not_of(" \t") + 1);
}

// UTILS FUNCTION TO CLEAN CONFIG FILE
bool ConfigParser::cleanEmptyLines(std::string &line) {
	if (line.empty() || line.length() == 0 || line == "\n") {
		line.clear();
		return (true);
	} else
		return (false);
}

void ConfigParser::closeFile(std::ifstream& file) {
	file.close();
	if (file.fail()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: could not close config file.");
	} else if (file.bad()) {
		throw ConfigParser::ConfigParserException("ERROR PARSING: Critical I/O error! Bad config file");
	}
}

void ConfigParser::printLines() {
	for (std::vector<std::string>::iterator it = this->_lines.begin(); it != this->_lines.end(); it++) {
		std::cout << *it << std::endl;
	}
}

ConfigParser::ConfigParserException::ConfigParserException(const std::string &msg) : message(msg) {}

const char* ConfigParser::ConfigParserException::what() const throw() {
	return message.c_str();
}

void setServers(std::vector<Server> servers) {
	// this->_servers = servers;
}

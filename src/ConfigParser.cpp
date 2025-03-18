#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser() {
}

ConfigParser::ConfigParser(const std::string& filepath) {
	this->_filepath = filepath;
	readConfigToInput();
	parseInputToTokens();
	printTokens();
	parseTokenToConfig();
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

std::string readFile(std::ifstream& config_file) {
	std::vector<std::string> lines;
	std::string line;
	while (config_file.peek() != EOF) {
		std::getline(config_file, line);
		cleanComments(line);
		lines.push_back(line);
		lines.push_back("\n");
	}
	return (std::accumulate(lines.begin(), lines.end(), std::string("")));
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
		this->_input = readFile(config_file);
		closeFile(config_file);
	} catch (std::exception &e) {
		throw ConfigParser::ConfigParserException(e.what());
	}
}

void ConfigParser::printInput() {
	std::cout << this->_input << std::endl;
}

void ConfigParser::printTokens() {
	std::unordered_map<tokenType, std::string> tokenColorMap = {
		{INIT, BG_BRIGHT_WHITE},
		{WHITE_SPACE, BG_BRIGHT_WHITE},
		{SERVER, BG_BRIGHT_BLUE},
		{LOCATION, BG_BLUE},
		{BLOCK_OPEN, BG_BRIGHT_CYAN},
		{BLOCK_CLOSE, BG_BRIGHT_GREEN},
		{SEMICOLON, BG_BRIGHT_YELLOW},
		{STRING, BG_BRIGHT_MAGENTA},
		{NUMBER, BG_YELLOW},
		{URL, BG_CYAN},
		{PATH, BG_GREEN},
		{OPERATOR, BG_PURPLE},
		{EOF_TOKEN, BG_RED}
	};
	for (const auto &it : _tokens) {
		std::string color = tokenColorMap.count(it.type) ? tokenColorMap[it.type] : BG_RED;
		if (it.value != "\n")
			std::cout << color << it.value << RESET;
		else
			std::cout << it.value << RESET;
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
	} else if (newToken.value == "location") {
		newToken.type = LOCATION;
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
	static const std::regex op_regex(R"([=~!^][=~!^|*]*)");
	newToken.type = INIT;
	newToken.itEnd = this->_input.begin();
	while (newToken.type != EOF_TOKEN) {
		newToken = getNextToken(newToken, url_regex, path_regex, op_regex);
		this->_tokens.push_back(newToken);
	}
}

void	ConfigParser::getTokenPos(token token, int &line, int &col) {
	line = 1;
	col = 1;
	for (std::string::iterator i = this->_input.begin(); i != this->_input.end() && i != token.itStart; i++) {
		col++;
		if (*i == '\n') {
			line++;
			col = 0;
		}
	}
}

void	ConfigParser::errorToken(token token, std::string msg) {
	int line;
	int col;
	getTokenPos(token, line, col);
	throw ConfigParser::ConfigParserException("Unexpected token at Ln " + std::to_string(line) + ", Col " + std::to_string(col) + " " + msg);
}

void ConfigParser::parseTokenToDirective(std::vector<token>::iterator &it, Config &newServer) {
	std::string key = it->value;
	std::vector<token>::iterator key_it = it;
	std::vector<std::string> values;
	moveOneTokenSafly(it);
	for (;it != this->_tokens.end(); it++) {
		if (it->type < STRING)
			break;
		values.push_back(it->value);
	}
	if (it->type != SEMICOLON)
		errorToken(*it, "Expected: ;");
	if (newServer.setDirective(key, values) == -1) {
		errorToken(*key_it, "Info: duplicate directive key.");
	}
}

void	ConfigParser::moveOneTokenSafly(std::vector<token>::iterator &it) {
	it++;
	if (it == this->_tokens.end())
		errorToken(*it, "Unexpected EOF!");
	else if (it->type == EOF_TOKEN)
		errorToken(*it, "Unexpected EOF!");
}

void ConfigParser::parseTokenToLocDir(std::vector<token>::iterator &it, Location &loc) {
	std::string key = it->value;
	std::vector<token>::iterator key_it = it;
	std::vector<std::string> values;
	moveOneTokenSafly(it);
	for (;it != this->_tokens.end(); it++) {
		if (it->type < STRING)
			break;
		values.push_back(it->value);
	}
	if (it->type != SEMICOLON)
		errorToken(*it, "Expected: ;");
	if (loc.directives.find(key) != loc.directives.end())
		errorToken(*key_it, "Info: duplicate directive key.");
	loc.directives[key] = values;
}

void ConfigParser::parseTokenToLocation(std::vector<token>::iterator &it, Config &newServer) {
	moveOneTokenSafly(it);
	Location newloc;
	std::string path_key;

	if (it->value == "=") {
		newloc.strict_match = 1;
		moveOneTokenSafly(it);
	} else
		newloc.strict_match = 0;

	if (it->type == PATH) {
		path_key = it->value;
		moveOneTokenSafly(it);
	} else
		errorToken(*it, "Expected: PATH");

	if (it->type == BLOCK_OPEN)
		moveOneTokenSafly(it);
	else
		errorToken(*it, "Expected: {");

	for (;it != this->_tokens.end(); it++) {
		if (it->type == STRING)
			parseTokenToLocDir(it, newloc);
		else if (it->type == BLOCK_CLOSE) {
			// moveOneTokenSafly(it);
			break;
		}
		else
			errorToken(*it, "Expected STRING or }");
	}
	newServer.setLocation(path_key, newloc);
}

void ConfigParser::parseTokenToServer(std::vector<token>::iterator &it) {
	moveOneTokenSafly(it);
	if (it == this->_tokens.end() || it->type != BLOCK_OPEN)
		errorToken(*it, "{");
	else if (it->type == BLOCK_OPEN)
		moveOneTokenSafly(it);
	Config newServer;
	for (;it != this->_tokens.end(); ++it) {
		if (it->type == STRING)
			parseTokenToDirective(it, newServer);
		else if (it->type == LOCATION)
			parseTokenToLocation(it, newServer);
		else if (it->type == BLOCK_CLOSE) {
			it++;
			break;
		}
		else {
			std::cout << "TEST: " << it->value << std::endl;
			errorToken(*it, "Expected STRING, LOCATION or }");
		}
	}
	newServer.printConfig();
	this->_configs.push_back(newServer);
}

void	ConfigParser::eraseWhitespaceToken() {
	for (std::vector<token>::iterator it = this->_tokens.begin(); it != this->_tokens.end(); ++it) {
		if (it->type == WHITE_SPACE) {
			this->_tokens.erase(it);
			it--;
		}
	}
}

void	ConfigParser::parseTokenToConfig() {
	eraseWhitespaceToken();
	// printTokens();
	for (std::vector<token>::iterator it = this->_tokens.begin(); it != this->_tokens.end(); ++it) {
		if (it->type == SERVER) {
			parseTokenToServer(it);
			// std::cout << it->type << ":" << it->value << std::endl;
			it--;
		}
		else if (it->type != EOF_TOKEN) {
			errorToken(*it, "Expected: server");
		}
	}
	if (this->_configs.empty())
		throw ConfigParser::ConfigParserException("Missing any Server config in .conf file.");
		// ERROR
}
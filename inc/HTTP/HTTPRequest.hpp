#pragma once

# include <string>
# include <unordered_map>
# include <vector>

struct HTTPRequest
{
	std::vector<std::string>	host;
	std::vector<std::string>	subdir;
	std::string 				method;
	std::string 				request_target;
	std::string 				protocol;
	std::string 				body;
	bool						invalidRequest;
	bool						dir_list;
	int							status_code;
	std::unordered_map<std::string, std::string> headers;
};

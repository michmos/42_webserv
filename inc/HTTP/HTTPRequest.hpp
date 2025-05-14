#pragma once

# include <string>
# include <unordered_map>
# include <vector>

struct HTTPRequest
{
	std::vector<std::string>	host;
	std::string 				method;
	std::string 				request_target;
	std::string 				request_uri;
	std::string 				protocol;
	std::string 				body;
	bool						invalidRequest = false;
	bool						dir_list = false;
	bool						redir_ = false;
	int							status_code = 0;
	std::unordered_map<std::string, std::string> headers;
};

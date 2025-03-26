#pragma once

#include <string>
#include <unordered_map>

struct HTTPRequest
{
	std::vector<std::string>	host;
	std::string 				method;
	std::string 				request_target;
	std::string 				protocol;
	std::string 				body;
	bool						invalidRequest;
	std::unordered_map<std::string, std::string> headers;
};

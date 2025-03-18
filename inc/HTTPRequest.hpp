#pragma once

#include <string>
#include <unordered_map>

struct HTTPRequest
{
	std::string method;
	std::string request_target;
	std::string protocol;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
	bool invalidRequest;
};

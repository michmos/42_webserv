#pragma once

#include <string>
#include <vector>

struct HTTPRequestConfig
{
	std::vector<std::string>	acceptedMethods;
	std::string					redirection;
	std::string					searchPath; 
	bool						isDirectoryListingEnabled;
	std::string					indexPage;
};

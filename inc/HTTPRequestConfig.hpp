#pragma once

#include <string>
#include <vector>


// NOT needed
struct HTTPRequestConfig
{
	bool						set_;
	std::vector<std::string>	acceptedMethods;
	std::string					redirection;
	std::string					searchPath; 
	bool						isDirectoryListingEnabled;
	std::string					indexPage;
};

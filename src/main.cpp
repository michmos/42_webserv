#include "../inc/Webserv/Webserv.hpp"

// int	main() {
// 	try {
// 		// Server	server(INADDR_ANY, htons(8080));
		
// 		// server.runServer();

// 	} catch (std::runtime_error &e) {
// 		std::cerr << "Runtime error occurred: " << e.what() << std::endl;
// 	} catch (std::invalid_argument &e) {
// 		std::cerr << "Invalid argument error occurred: " << e.what() << std::endl;
// 	} catch (...) {
// 		std::cerr << "Other error occurred" << std::endl;
// 	}

// 	return (0);
// }

int main (int argc, char **argv) {
	std::string path;

	try
	{
		std::cerr << argc << std::endl;
		if (argc == 1)
			path = "configs/default.conf";
		else if (argc == 2)
			path = argv[1];
		else
		{
			std::cerr << "Wrong amount of config files given" << std::endl;
			return (1);
		}
		Webserv	server(path);
	}
	catch (std::runtime_error &e)
	{
		std::cerr << "Runtime error occurred: " << e.what() << std::endl;
	}
	catch (std::invalid_argument &e) {
		std::cerr << "Invalid argument error occurred: " << e.what() << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "something went wrong" << std::endl;
		return (1);
	}
	return (0);
}
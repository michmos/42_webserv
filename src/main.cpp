#include "../inc/Webserv/Webserv.hpp"

int	save_main(int argc, char **argv) {
	std::string path = "configs/";

	if (argc == 1)
		path += "default.conf";
	else if (argc == 2)
		path += argv[1];
	else
		throw std::invalid_argument("Wrong amount config files given");

	Webserv	server(path);
	return (0);
}

int main (int argc, char **argv) {
	try {
		return (save_main(argc, argv));
	}
	catch (std::runtime_error &e) {
		std::cerr << "Runtime error occurred: " << e.what() << std::endl;
	}
	catch (std::invalid_argument &e) {
		std::cerr << "Invalid argument error occurred: " << e.what() << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "Error: Something else went wrong" << std::endl;
	}
	return (01);
}
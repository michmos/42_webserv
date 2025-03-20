#include "Server.hpp"

int	main() {
	try {
		Server	server(INADDR_ANY, htons(8080));
		
		server.runServer();

	} catch (std::runtime_error &e) {
		std::cerr << "Runtime error occurred: " << e.what() << std::endl;
	} catch (std::invalid_argument &e) {
		std::cerr << "Invalid argument error occurred: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Other error occurred" << std::endl;
	}

	return (0);
}

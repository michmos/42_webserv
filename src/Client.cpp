#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "../inc/Socket.hpp"

int	main() {
	try {
		Socket	server;

		server.sConnect(inet_addr("127.0.0.1"), htons(8080));
		const char *msg = "hello world";
		write(server.getFd(), msg, strlen(msg));
		std::cout << "message written" << std::endl;
		close(server.getFd());
	} catch (...) {
		std::cerr << "Error occurred" << std::endl;
	}
	return (0);
}

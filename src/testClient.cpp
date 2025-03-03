#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "../inc/Socket.hpp"

int	main() {
	try {
		Socket	server;

		server.sConnect(inet_addr("127.0.0.1"), htons(8080));
		std::cout << "|CLIENT| connected succesfully" << std::endl;
		std::string buff;
		while (std::getline(std::cin, buff) && buff != "exit") {
			write(server.getFd(), buff.c_str(), strlen(buff.c_str()));
			std::cout << "|CLIENT| message written" << std::endl;
		}
		close(server.getFd());
	} catch (...) {
		std::cerr << "|CLIENT| Error occurred" << std::endl;
	}
	return (0);
}

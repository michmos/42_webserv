#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

void	setFdFlag(int fd, int flag) {
	int flags = fcntl(fd, F_GETFL);
	if (fcntl(fd, F_SETFL, flags | flag) == -1)
		throw std::runtime_error(std::string("fcntl()") + strerror(errno));
}

#include "../inc/Socket.hpp"
#include "../inc/Epoll.hpp"

int	main() {
	try {
		Socket	listen;
		Epoll	ep;

		listen.sBind(INADDR_ANY, htons(8080));
		listen.sListen(5);
		setFdFlag(listen.getFd(), O_NONBLOCK);
		ep.add(listen.getFd(), EPOLLIN);

		while (true) {
			auto events = ep.wait();
			for (auto ev : events) {
				std::cout << "client: " << ev.data.fd << " "
					<< ((ev.events & EPOLLIN) ? "EPOLLIN " : " ")
					<< ((ev.events & EPOLLOUT) ? "EPOLLOUT " : " ")
					<< ((ev.events & (EPOLLHUP | EPOLLERR)) ? "EPOLLHUP | EPOLLERR" : "") << std::endl;

				if (ev.data.fd == listen.getFd()) {
					int newClient = listen.sAccept();
					ep.add(newClient, EPOLLIN | EPOLLOUT);
					std::cout << "New client registered: " << newClient << std::endl;
				} else {
					// do someting wit existing client
					if (ev.events & EPOLLIN) {
						char buff[100] = { '\0' };
						int bRead = read(ev.data.fd, buff, 100);
						std::cout << "Received following message: " << buff << std::endl;
						// TODO: check read -1
						if (bRead < 100) {
							ep.del(ev.data.fd);
							close(ev.data.fd);
							std::cout << "Client connection closed: " << ev.data.fd << std::endl;
						}
					} else if (ev.events & (EPOLLHUP | EPOLLERR)) {
						ep.del(ev.data.fd);
						close(ev.data.fd);
						std::cout << "Client connection closed: " << ev.data.fd << std::endl;
					} 				}
			}
		}
	} catch (std::runtime_error &e) {
		std::cerr << "Runtime error occurred: " << e.what() << std::endl;
	} catch (std::invalid_argument &e) {
		std::cerr << "Invalid argument error occurred: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Other error occurred" << std::endl;
	}

	return (0);
}

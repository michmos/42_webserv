#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>

// size of event buffer for epoll_wait
#define MAX_EVENTS 10

#define EP_WAIT_TIMEOUT 5


// maybe throw exception instead
int	setFdFlag(int fd, int flag) {
	int flags = fcntl(fd, F_GETFL);
	if (fcntl(fd, F_SETFL, flags | flag) == -1)
		return (-1);
	return (0);
}

// throw exceptions
int	createListenSock(void) {
	int	listen_sock;

	if((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket()");
		return (EXIT_FAILURE);
	}
	if (setFdFlag(listen_sock, O_NONBLOCK) == -1) {
		return (EXIT_FAILURE);
	}

	struct sockaddr_in	srvrAddr;
	srvrAddr.sin_family = AF_INET;
	srvrAddr.sin_addr.s_addr = INADDR_ANY;
	srvrAddr.sin_port = htons(8080);
	if (bind(listen_sock, (const struct sockaddr*)&srvrAddr, sizeof(srvrAddr)) == -1) {
		perror("socket()");
		return (EXIT_FAILURE);
	}

	// TODO: double check backlog size
	if(listen(listen_sock, 5) == -1) {
		perror("listen()");
		return (EXIT_FAILURE);
	}
	return (listen_sock);
}

// throw exceptions
int	epoll_add(int epFd, int fd, u_int32_t events) {
	epoll_event	ev;

	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(epFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_clt(EPOLL_CTL_ADD)");
		return (EXIT_FAILURE);
	}
	return (0);
}

int	main() {
	int listen_sock = createListenSock();
	// TODO: double check epoll indication size
	int epFd = epoll_create(5);
	epoll_add(epFd, listen_sock, EPOLLIN);

	while (true) {
		epoll_event	events[MAX_EVENTS];
		int ready = epoll_wait(epFd, events, MAX_EVENTS, EP_WAIT_TIMEOUT);
		if (ready == -1) {
			perror("epoll_wait()");
			return (EXIT_FAILURE);
		}
		for (int i = 0; i < ready; ++i) {
			if (events[i].data.fd == listen_sock) {
				int newClient = accept(listen_sock, nullptr, nullptr);
				if (newClient == -1) {
					perror("accept()");
					return (EXIT_FAILURE);
				}
				epoll_add(epFd, newClient, EPOLLIN | EPOLLOUT);
			} else {
				// do something with existent client
			}
		}
	}

	return (0);
}

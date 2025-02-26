
#include <unordered_map>
#include <string>
#include <iostream>
#include "Socket.hpp"
#include "Epoll.hpp"
#include "Client.hpp"

class Server {
public:
	Server(u_int32_t addr, u_int16_t port);
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	~Server();

	void	runServer();

private:
	Epoll	_ep;
	Socket	_listen;
	std::unordered_map<int, Client>	_clients;

	void	addClient();
	void	delClient(int fd);
	void	handleClient(u_int32_t events, int fd);
};

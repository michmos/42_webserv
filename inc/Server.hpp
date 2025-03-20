#pragma once

#include <string>

class Server {
public:
	Server(int fd);
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	~Server();

	int	getFd() const;
	const std::string& getName() const;
	void	setName(const std::string& name);

private:
	int				_fd;
	std::string		_name;
	// Config		_conf;
};

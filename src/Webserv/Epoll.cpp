#include "../../inc/Webserv/Epoll.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>

Epoll::Epoll() : 
	_epFd(epoll_create(EPOLL_START_SIZE)),
	_timeout(TIMEOUT_DFLT)
{
	if (_epFd == -1) {
		throw std::runtime_error(std::string("epoll_create(): ") + strerror(errno));
	}
	_events.reserve(MAX_EVENTS);
}

// add a file descriptor to epoll
// - fd: 	fd to monitor
// - data:	other data - can be used to store associated fd
void	Epoll::add(int fd, int data, u_int32_t events) {
	std::cerr << "add fd: " << fd << std::endl;

	_eventDataStorage[fd] = {fd, data};

	epoll_event	ev;
	ev.events = events;
	ev.data.ptr = &_eventDataStorage.at(fd);

	if (epoll_ctl(_epFd.get(), EPOLL_CTL_ADD, fd, &ev) == -1) {
		throw std::runtime_error(std::string("epoll_ctl(add): ") + strerror(errno));
	}
}

void	Epoll::add(int fd, u_int32_t events) {
	add(fd, -1, events);
}

void	Epoll::mod(int fd, int data, u_int32_t events) {
	_eventDataStorage[fd] = {fd, data};

	epoll_event	ev;
	ev.events = events;
	ev.data.ptr = &_eventDataStorage.at(fd);
	if (epoll_ctl(_epFd.get(), EPOLL_CTL_MOD, fd, &ev) == -1) {
		throw std::runtime_error(std::string("epoll_ctl(mod): ") + strerror(errno));
	}
}

const struct epollEventData&	Epoll::getEventData(struct epoll_event& ev) {
	if (ev.data.ptr == nullptr) {
		throw std::runtime_error("getEventData(): data.ptr is null");
	}
	return (*static_cast<epollEventData*>(ev.data.ptr));
}

void	Epoll::mod(int fd, u_int32_t events) {
	mod(fd, -1, events);
}

void	Epoll::del(int fd) {
	std::cerr << "del fd: " << fd << std::endl;

	auto it = _eventDataStorage.find(fd);
	if (it != _eventDataStorage.end()) {
		_eventDataStorage.erase(it);
	}

	if (epoll_ctl(_epFd.get(), EPOLL_CTL_DEL, fd, nullptr) == -1) {
		throw std::runtime_error(std::string("epoll_ctl(del): ") + strerror(errno));
	}
}

const std::vector<struct epoll_event>&	Epoll::wait() {
	int	ready;

	if ((ready = epoll_wait(_epFd.get(), _events.data(), MAX_EVENTS, _timeout)) == -1) {
		throw std::runtime_error(std::string("epoll_wait(): ") + strerror(errno));
	}
	_events.erase(_events.begin() + ready, _events.end());
	return (_events);
}

void	Epoll::setTimeout(int timeout) noexcept {
	_timeout = timeout;
}


int		Epoll::getEpFd() const {
	return (_epFd.get());
}

Epoll::~Epoll() noexcept {
}

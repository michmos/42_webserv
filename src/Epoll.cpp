
#include "../inc/Epoll.hpp"

Epoll::Epoll(int size) : _epFd(epoll_create(size)), _timeout(TIMEOUT_DFLT) {
	if (_epFd == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Epoll::add(int fd, u_int32_t events) const {
	epoll_event	ev;

	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(_epFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Epoll::mod(int fd, u_int32_t events) const {
	epoll_event	ev;

	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(_epFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Epoll::del(int fd) const {
	if (epoll_ctl(_epFd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Epoll::wait() {
	if (epoll_wait(_epFd, _events, _maxEvents, _timeout) == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void	Epoll::setTimeout(int timeout) noexcept {
	_timeout = timeout;
}

Epoll::~Epoll() noexcept {
}

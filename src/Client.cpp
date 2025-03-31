
// #include "../inc/Client.hpp"

// // TODO: implement move semantics for SharedFd
// Client::Client(
// 		SharedFd clientFd,
// 		SharedFd serverFd,
// 		std::function<void(struct epoll_event, const SharedFd&)> addToEpoll_cb,
// 		std::function<const Config* const (const SharedFd& serverSock, const std::string& serverName)> getConfig_cb
// 	) : 
// 	_clientSock(clientFd),
// 	_serverSock(serverFd),
// 	_addToEpoll_cb([this, addToEpoll_cb = std::move(addToEpoll_cb)] (struct epoll_event ev) {
// 		addToEpoll_cb(ev, _clientSock);
// 	}),
// 	_setConfig_cb( [this,  getConfig_cb = std::move(getConfig_cb)](const std::string& serverName) {
// 		_config = getConfig_cb(this->_serverSock, serverName);
// 	}),
// 	_state(REQUEST),
// 	_config(nullptr)
// {
// }

// Client::~Client() {
// }

// void	Client::handle(struct epoll_event epollData) {
// 	switch(_state) {
// 		case REQUEST:
// 			_setConfig_cb("NAME"); // TODO: use actual name instead
// 			break;
// 		case RESPONSE:
// 			break;
// 		case CGI_PROCESS:
// 			break;
// 		default:
// 			break;
// 	}
// }

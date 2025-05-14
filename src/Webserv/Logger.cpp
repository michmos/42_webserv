
#include "../../inc/Webserv/Logger.hpp"
#include "../../inc/Webserv/Colors.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <ostream>

// declare static variables
std::unique_ptr<Logger> Logger::instance_ = nullptr;
std::once_flag Logger::o_flag_;

Logger::Logger() : enabled_(true), crntLogLevel_(LOG_DEBUG) {};

Logger::~Logger() {};

std::string getTimeStamp() {
	time_t now;

	now = time(0);
	char buffer[100];
	if (std::strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime(&now))) {
		return (std::string(buffer));
	}
	return {};
}

Logger& Logger::getInstance() {
	std::call_once(o_flag_, []() { instance_.reset(new Logger()); });
	return (*instance_);
}

void	Logger::log(LogLevel level, const std::string& msg) {
	std::string timeStamp = getTimeStamp();

	if (!enabled_ || level < crntLogLevel_)
		return;

	switch (level) {
		case LOG_DEBUG:
			std::cerr << YELLOW << timeStamp << std::left << std::setw(13) << " [DEBUG]" << msg  << RESET << std::endl;
			break;
		case LOG_WARN:
			std::cout << ORANGE << timeStamp << std::left << std::setw(13) << " [WARN]	" << msg  << RESET << std::endl;
			break;
		case LOG_ERROR:
			std::cout << RED << timeStamp << std::left << std::setw(13) << " [ERROR]" << msg  << RESET << std::endl;
			break;
		case LOG_FATAL:
			std::cout << RED << timeStamp << std::left << std::setw(13) << " [FATAL]" << msg  << RESET << std::endl;
			break;
		case LOG_INFO:
			std::cout << LIGHT_GRAY << timeStamp << std::left << std::setw(13) << " [INFO]" << msg  << RESET << std::endl;
			break;
		case LOG_REQUEST:
			std::cout << BLUE << timeStamp << std::left << std::setw(13) << " [REQUEST]" << msg  << RESET << std::endl;
			break;
		case LOG_RESPONSE:
			std::cout << LIGHT_BLUE << timeStamp << std::left << std::setw(13) << " [RESPONSE]" << msg  << RESET << std::endl;
			break;
		default:
			break;
	}
}


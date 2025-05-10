#pragma once

#include <memory>
#include <mutex>

enum LogLevel {
	LOG_DEBUG,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
	LOG_INFO,
	LOG_REQUEST,
	LOG_RESPONSE
};

class Logger {
private:
	Logger();

	bool		enabled_;
	LogLevel	crntLogLevel_;
	static std::unique_ptr<Logger>	instance_;
	static std::once_flag			o_flag_;

public:
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;
	~Logger();

	static Logger& getInstance();
	void	log(LogLevel level, const std::string& msg);

	inline void	setLogLevel(LogLevel level) { crntLogLevel_ = level; }
	inline void	enable() { enabled_ = true; }
	inline void	disable() { enabled_ = false; }
};


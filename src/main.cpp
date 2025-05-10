#include "../inc/Webserv/Webserv.hpp"
#include "../inc/Webserv/Logger.hpp"

std::atomic<bool> keep_alive(true);

void	signal_handler(int signum)
{
	if (signum == 13 || signum == SIGINT)
		keep_alive = false;
}

int	save_main(int argc, char **argv) {
	if (argc > 2) {
		Logger::getInstance().log(LOG_ERROR, "Usage: ./webserv <path_to_config>");
		exit(EXIT_FAILURE);
	}

	std::string path = (argc == 2) ? argv[1] : DEFAULT_CONFIGFILE;

	std::signal(SIGINT, signal_handler);

	Logger::getInstance().log(LOG_INFO, "Webserver starting ... with config: " + path);
	Webserv	webserver(path);
	Logger::getInstance().log(LOG_INFO, "Webserver has started");
	webserver.eventLoop();
	return (0);
}


int main (int argc, char **argv) {
	try
	{
		Logger::getInstance().setLogLevel(LOG_ERROR);
		#ifdef DEBUG
		Logger::getInstance().setLogLevel(LOG_DEBUG);
		Logger::getInstance().log(LOG_DEBUG, "Debugging info active");
		#endif
		return (save_main(argc, argv));
	}
	catch (std::runtime_error &e) {
		Logger::getInstance().log(LOG_FATAL, "Runtime error occurred: " + std::string(e.what()));
	}
	catch (std::invalid_argument &e) {
		Logger::getInstance().log(LOG_ERROR, "Invalid argument error occurred: " + std::string(e.what()));
	}
	catch (CGIException &e) {
		Logger::getInstance().log(LOG_ERROR, "CGI error occurred: " + std::string(e.what()));
	}
	catch (std::exception &e) {
		Logger::getInstance().log(LOG_FATAL, "Error occurred: " + std::string(e.what()));
	}
	catch (...) {
		Logger::getInstance().log(LOG_FATAL, "Unknown error occurred");
	}
	return (1);
}

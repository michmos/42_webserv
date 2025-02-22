#include "../inc/Webserv.hpp"
#include "../inc/ConfigParser.hpp"

int main(int argc, char **argv)
{
	if (argc == 1 || argc == 2) {
		try
		{
			std::string		config;
			config = (argc == 1 ? "configs/default.conf" : argv[1]);
			ConfigParser config_parser(config);
			config_parser.readConfigToLines();
			config_parser.printLines();
			config_parser.parseConfigLines();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
    }
    else
	{
		std::cerr << RED << "ERROR: " << RESET << "Invalid number of arguments" << std::endl;
		return (1);
	}
    return (0);
}

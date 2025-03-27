#include "../inc/CGI.hpp"

void	CGI::throwException(const char *msg) {
	std::cerr << "Error: " << msg << std::endl;
	std::perror(msg);
	closeAllPipes();
	throw std::exception();
}

void	CGI::throwExceptionExit(const char *msg) {
	std::cerr << "Error: " << msg << std::endl;
	closeAllPipes();
	exit(EXIT_FAILURE);
}

/// @brief closes pipe_from_child and pipe_to_child 
void	CGI::closeAllPipes(void) {
	if (pipe_from_child_[WRITE] != -1)
		close(pipe_from_child_[WRITE]);
	if (pipe_from_child_[READ] != -1)
		close(pipe_from_child_[READ]);
	if (pipe_to_child_[WRITE] != -1)
		close(pipe_to_child_[WRITE]);
	if (pipe_to_child_[READ] != -1)
		close(pipe_to_child_[READ]);
}

/// @brief closes pipes and set them to -1
void	CGI::closeTwoPipes(int &pipe1, int &pipe2) {
	if (pipe1 != -1)
	{
		close(pipe1);
		pipe1 = -1;
	}
	if (pipe2 != -1)
	{
		close(pipe2);
		pipe2 = -1;
	}
}

/// @brief creating a vector<char*> for transfor to an array of * to strings ascommand-line arguments for execve.
void	CGI::createArgvVector(std::vector<char*> &argv_vector, const std::string &executable) {
	argv_vector.push_back(const_cast<char *>(executable.c_str()));
	argv_vector.push_back(NULL);
}

/**
 * @brief fills the vector with the variables as env and add also a gateway key for the cgi protocol
 * @param env_c_vector vector<char*> with env variables
 * @param env_vector vector<string,string> with env variables
 */
void	CGI::createEnvCharPtrVector(std::vector<char*> &env_c_vector, std::vector<std::string> &env_vector) {
	env_vector.push_back(const_cast<char*>("GATEWAY=CGI/1.1"));
	env_vector.push_back(const_cast<char*>("SERVER_PROTOCOL=HTTP/1.1"));
	for (auto& str : env_vector)
	{
		std::cerr << "env: " << str << std::endl;
		env_c_vector.push_back(const_cast<char*>(str.c_str()));
	}
	env_c_vector.push_back(NULL);
}

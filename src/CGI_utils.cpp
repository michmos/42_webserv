#include "CGI.hpp"

void	CGI::throwException(const char *msg) {
	std::cerr << "Error: " << msg << std::endl;
	std::perror(msg);
	closeAllPipes();
	throw std::exception();
}

void	CGI::throwExceptionExit(const char *msg)  {
	std::cerr << "Error: " << msg << std::endl;
	closeAllPipes();
	exit(EXIT_FAILURE);
}

void	CGI::closeAllPipes(void) {
	if (m_pipe_from_child[WRITE] != -1)
		close(m_pipe_from_child[WRITE]);
	if (m_pipe_from_child[READ] != -1)
		close(m_pipe_from_child[READ]);
	if (m_pipe_to_child[WRITE] != -1)
		close(m_pipe_to_child[WRITE]);
	if (m_pipe_to_child[READ] != -1)
		close(m_pipe_to_child[READ]);
}

/**
 * @brief set pipes to -1 and then open both (to and from child)
*/
void	CGI::setPipes(void) {
	std::memset(m_pipe_from_child, -1, sizeof(m_pipe_from_child));
	std::memset(m_pipe_to_child, -1, sizeof(m_pipe_to_child));
	if (pipe(m_pipe_to_child) < 0)
		throwException("pipe failed");
	if (pipe(m_pipe_from_child) < 0)
		throwException("pipe failed");
}

/**
 * @brief closes pipes and set them to -1
 */
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

/**
 * @brief creating a vector<char*> for transfor to an array of * to strings ascommand-line arguments for execve.
 */
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
		// env_c_vector.push_back(const_cast<char*>((std::string("HTTP_") + str).c_str()));
		env_c_vector.push_back(const_cast<char*>(str.c_str()));
	}
	env_c_vector.push_back(NULL);
}
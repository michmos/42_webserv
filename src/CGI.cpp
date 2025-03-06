#include "CGI.hpp"

/**
 * @brief forks the process to execve the CGI, with POST sends buffer to child
 * @param executable const string CGI filename
 * @param env_vector char* vector with key-values as env argument to CGI
 */
CGI::CGI(const std::string &executable, std::vector<std::string> env_vector, const std::string &post_data)
{
	std::cout << std::flush;
	setPipes();
	m_pid = fork();
	if (m_pid < 0)
		throwException("Fork failed");
	else if (m_pid == 0) 
	{
		closeTwoPipes(m_pipe_to_child[WRITE], m_pipe_from_child[READ]);
		if (dup2(m_pipe_to_child[READ], STDIN_FILENO) < 0)
			throwExceptionExit("dub2 failed");
		if (dup2(m_pipe_from_child[1], STDOUT_FILENO) < 0)
			throwExceptionExit("dub2 failed");

		std::vector<char*>	argv_vector;
		std::vector<char*>	env_c_vector;
		createArgvVector(argv_vector, executable);
		createEnvCharPtrVector(env_c_vector, env_vector);

		if (execve(executable.c_str(), argv_vector.data(), env_c_vector.data()) == -1)
		{
			std::cerr << "Error: Execve failed: " << std::strerror(errno) << std::endl;
			closeTwoPipes(m_pipe_to_child[READ], m_pipe_from_child[WRITE]);
		}
		exit(1);
	}
	closeTwoPipes(m_pipe_to_child[READ], m_pipe_from_child[WRITE]);
	sendDataToStdin(post_data);
	waitForChild();
	closeAllPipes();
	getResponseFromCGI();
	if (!isNPHscript(executable))
		rewriteResonseFromCGI();
}

CGI::~CGI(void) {}

std::string CGI::getResponse(void) {
	return (m_response);
}

/**
 * @brief extract statuscode from CGI response
 * @return int with statuscode or zero if not found
 */
int	CGI::getStatusCodeFromResponse(void) {
	std::regex	status_code_regex(R"(HTTP/1.1 (\d+))");
	std::smatch	match;
	int			status_code = 0;

	if (!m_response.empty() && std::regex_search(m_response, match, status_code_regex))
	{
		std::string to_string = match[1];
		if (to_string.size() < 9)
			status_code = std::stoi(match[1]);
		else
			status_code = 500;
	}
	else
		std::cerr << "Error: No response or statuscode is found in response" << std::endl;
	return (status_code);
}

/**
 * @brief waits 5 sec for CGI and if passed but not returned, a timeout will set and the process will be killed
 */
void	CGI::waitForChild(void) {
	time_t	start_time = std::time(nullptr);

	while (std::time(nullptr) - start_time < 5)
	{
		if (waitpid(m_pid, &m_status, WNOHANG) > 0)
			return ;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cerr << "Timeout in CGI, process will be killed" << std::endl;
	if (kill(m_pid, SIGKILL) == -1)
		std::cerr << "Error kill: " << std::strerror(errno) << std::endl;
}

/**
 * @brief checks response status from CGI and receives header (and body) from pipe
 * if statuscode is not set it wil generate a Internal Server Error
 */
void	CGI::getResponseFromCGI(void) {
	if (WIFEXITED(m_status)) {
		int return_value = WEXITSTATUS(m_status);
		m_response = receiveBuffer();
		if (return_value != 0) {
			int status_code = getStatusCodeFromResponse();
			std::cerr << "status_code; " << status_code << std::endl;
			// compare with configfile error pages.
			std::cerr << "response" << m_response << std::endl;
		}
	}
	else
		m_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
}

/**
 * @brief checks if the executable file starts with nph_
 * @param executable absolute path to request target
 * @return true if nhp_ file, else false
 */
bool	CGI::isNPHscript( const std::string &executable )
{
	size_t		index;
	std::string	filename = "";

	index = executable.find_last_of('/');
	if (index != std::string::npos)
		filename = executable.substr(index + 1);
	else
		filename = executable;
	if (filename.size() > 4 && filename.substr(0, 4) == "nph_")
		return true;
	else
		return false;
}

/**
 * @brief Set up a response for the client after receiving the header from the CGI
 */
void	CGI::rewriteResonseFromCGI(void) {
	std::smatch	match;
	std::string	new_response = "";
	std::regex	r_content_type = std::regex(R"(Content-Type:\s+([^\r\n]+)\r\n)");
	std::regex	r_status = std::regex(R"(Status:\s+([^\r\n]+)\r\n)");
	std::regex	r_location = std::regex(R"(Location:\s+([^\r\n]+)\r\n)");
	
	if (std::regex_match(m_response, match, r_status) && match.size() == 2)
		new_response += "HTTP/1.1 " + std::string(match[1]) + "\r\n";
	if (std::regex_match(m_response, match, r_location) && match.size() == 2)
		new_response += "Location: " + std::string(match[1]) + "\r\n";
	if (std::regex_match(m_response, match, r_content_type) && match.size() == 2)
		new_response += "Content-Type: " + std::string(match[1]) + "\r\n";
	if (new_response.empty())
	{
		std::cerr << "Error: Received wrong formated header from CGI" << std::endl;
		m_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		return ;
	}
	new_response += "\r\n";

	std::size_t	index = m_response.find("\r\n\r");
	if (index != std::string::npos)
	{
		if (index + 4 < m_response.size())
			new_response += m_response.substr(index + 4) + "\r\n";
	}
	m_response = new_response;
}

/**
 * @brief compares executable extension with the allowed cgi scripts
 * @param path string with path and filename
 * @return string with path to executable program or empty when not correct
 */
std::string CGI::getScriptExecutable(const std::string &path)
{
	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return "/usr/bin/python3";
	// if (path.size() >= 4 && path.substr(path.size() - 4) == ".php")
	// 	return "/usr/bin/php";
	return "";
}

/**
 * @brief checks if executable is allowed and valid
 * @param path string with path and filename
 * @return bool if cgi script is valid
 */
bool CGI::isCgiScript(const std::string &path)
{
	std::string executable = getScriptExecutable(path);
	return !executable.empty();
}

/**
 * @brief receives a response from child with a headerfile and body to return to the client
 * @return string buffer with response from child or error msg that something went wrong
 */
std::string	CGI::receiveBuffer(void) {
	char buffer[1024];
	
	ssize_t bytesRead = read(m_pipe_from_child[READ], buffer, sizeof(buffer) - 1);
	if (bytesRead > 0)
		buffer[bytesRead] = '\0';
	else
	{
		if (bytesRead == -1)
			std::cerr << "Error read: " << std::strerror(errno) << std::endl;
		else
			std::cerr << "Error: no output read"; // what now?
		return std::string("HTTP/1.1 500 Internal Server Error\nContent-Type: text/html\n\r\n<html>\n") +
			"<head><title>Server Error</title></head><body><h1>Something went wrong</h1></body></html>";
	}
	return (buffer);
}

/**
 * @brief writes body to stdin for CGI and closes write end pipe
 * @param post_data string with body
 */
void	CGI::sendDataToStdin( const std::string &post_data) {
	ssize_t	readBytes;

	readBytes = write(m_pipe_to_child[WRITE], post_data.c_str(), post_data.size());
	if (readBytes != (ssize_t)post_data.size())
	{
		if (readBytes == -1)
			std::cerr << "Error write: " << std::strerror(errno) << std::endl;
		else
			std::cerr << "Error write: not written right amount" << std::endl;
	}
	close(m_pipe_to_child[WRITE]);
	m_pipe_to_child[WRITE] = -1;
}

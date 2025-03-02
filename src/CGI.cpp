#include "CGI.hpp"

static void	throwException(const char *msg) {
	std::perror(msg);
	throw std::exception();
}


static void	throwExceptionExit(const char *msg) {
	std::perror(msg);
	exit(EXIT_FAILURE);
}

std::string CGI::getResponse(void) {
	return (m_response);
}

void	CGI::createArgvVector(std::vector<char*> &argv_vector, const std::string &executable) {
	argv_vector.push_back(const_cast<char *>(executable.c_str()));
	argv_vector.push_back(NULL);
}

void	CGI::createEnvCharPtrVector(std::vector<char*> &env_c_vector, std::vector<std::string> &env_vector) {
	for (auto& str : env_vector)
	{
		std::cerr << str << std::endl;
		env_c_vector.push_back(const_cast<char*>(str.c_str()));
	}
	env_c_vector.push_back(NULL);
}

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
		std::cerr << "No response or statuscode is found in response" << std::endl;
	return (status_code);
}

void	CGI::waitForChild(void) {
	time_t	start_time = std::time(nullptr);

	while (std::time(nullptr) - start_time < 5)
	{
		if (waitpid(m_pid, &m_status, WNOHANG) > 0)
			return ;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cerr << "Timeout in CGI" << std::endl;
	kill(m_pid, SIGKILL);
}

void	CGI::responseFromCGI(void) {
	// python gives only back numbers between 0 - 255 so catching this is only handy when it is not 0
	// check for nph_ (Non-Parsed Headers)
	if (WIFEXITED(m_status)) {
		int return_value = WEXITSTATUS(m_status);
		std::cerr << "return_value; " << return_value << std::endl;
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
 * @brief forks the process to execve the CGI, with POST sends buffer to child
 * @param executable const string CGI filename
 * @param env_vector char* vector with key-values as env argument to CGI
 */
CGI::CGI(const std::string &executable, std::vector<std::string> env_vector, const std::string &post_data)
{
	std::cerr << "execve: " << executable << std::endl;
	std::cout << std::flush;
	if (pipe(m_pipe_to_child) < 0 || pipe(m_pipe_from_child) < 0)
		throwException("Pipe failed");
	m_pid = fork();
	if (m_pid < 0)
		throwException("Fork failed");
	else if (m_pid == 0) 
	{
		close(m_pipe_to_child[WRITE]);
		close(m_pipe_from_child[READ]);
		if (dup2(m_pipe_to_child[READ], STDIN_FILENO) < 0)
			throwExceptionExit("dub2 failed");
		if (dup2(m_pipe_from_child[1], STDOUT_FILENO) < 0)
			throwExceptionExit("dub2 failed");

		std::vector<char*>	argv_vector;
		std::vector<char*> env_c_vector;
		createArgvVector(argv_vector, executable);
		createEnvCharPtrVector(env_c_vector, env_vector);

		if (execve(executable.c_str(), argv_vector.data(), env_c_vector.data()) == -1)
		{	
			std::perror("execve failed");
			close(m_pipe_to_child[READ]);
			close(m_pipe_from_child[WRITE]);
		}
		exit(1);
	}
	close(m_pipe_to_child[READ]);
	close(m_pipe_from_child[WRITE]);
	sendBodyToStdin(post_data);
	waitForChild();
	responseFromCGI();
	if (executable.size() > 4 && executable.substr(0, 4) == "nph_")
	{
		// header and body in response!
	}
}

CGI::~CGI(void) {}

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
		std::perror("read failed or no output"); // what now?
		return std::string("HTTP/1.1 500 Internal Server Error\nContent-Type: text/html\n\r\n<html>\n") +
			"<head><title>Server Error</title></head><body><h1>Something went wrong</h1></body></html>";
	}
	return (buffer);
}

void	CGI::sendBodyToStdin( const std::string &post_data) {
	if (write(m_pipe_to_child[WRITE], post_data.c_str(), post_data.size()) != (ssize_t)post_data.size())
		std::perror("write failed");
	
	close(m_pipe_to_child[WRITE]);
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
 * @brief compares executable extension with the allowed cgi scripts
 * @param path string with path and filename
 * @return string with path to executable program or empty when not correct
 */
std::string CGI::getScriptExecutable(const std::string &path)
{
	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return "/usr/bin/python3";
	if (path.size() >= 4 && path.substr(path.size() - 4) == ".php")
		return "/usr/bin/php";
	return "";
}

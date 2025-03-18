#include "CGI.hpp"

void	CGI::add_pipe_events(void) {
	std::memset(&epoll_event_pipe_[0], 0, sizeof(epoll_event_pipe_[0]));
	std::memset(&epoll_event_pipe_[1], 0, sizeof(epoll_event_pipe_[1]));
	// epoll_event_pipe_[0].data.fd = pipe_to_child_[WRITE];
	epoll_event_pipe_[0].events = EPOLLOUT | EPOLLET;
	epoll_event_pipe_[1].data.fd = pipe_from_child_[READ];
	epoll_event_pipe_[1].events = EPOLLIN | EPOLLET;
}


void	CGI::addEventWithData(int epoll_fd) {
	std::shared_ptr<std::string>	buffer;
	
	setPipes();
	add_pipe_events();
	buffer = std::make_shared<std::string>(post_data_);
	epoll_event_pipe_[0].data.ptr = static_cast<void*>(buffer.get());;
	std::cerr << pipe_to_child_[WRITE] << " : " << pipe_from_child_[READ] << " epollfd: " << epoll_fd << std::endl;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_to_child_[WRITE], &epoll_event_pipe_[0]) == -1)
	{
		std::cerr << "Failed to add event to epoll" << errno << strerror(errno) << std::endl;
	}
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_from_child_[READ], &epoll_event_pipe_[1]) == -1)
	{
		std::cerr << "Failed to add event to epoll" << std::endl;
		return;
	}
}

CGI::CGI(const std::string &post_data) : post_data_(post_data) { }

/**
 * @brief forks the process to execve the CGI, with POST sends buffer to child
 * @param executable const string CGI filename
 * @param env_vector char* vector with key-values as env argument to CGI
 */
void	CGI::forkCGI(const std::string &executable, std::vector<std::string> env_vector) {
	std::cout << std::flush;
	pid_ = fork();
	if (pid_ < 0)
		throwException("Fork failed");
	else if (pid_ == 0) 
	{
		closeTwoPipes(pipe_to_child_[WRITE], pipe_from_child_[READ]);
		if (dup2(pipe_to_child_[READ], STDIN_FILENO) < 0)
			throwExceptionExit("dub2 failed");
		if (dup2(pipe_from_child_[1], STDOUT_FILENO) < 0)
			throwExceptionExit("dub2 failed");

		std::vector<char*>	argv_vector;
		std::vector<char*>	env_c_vector;
		createArgvVector(argv_vector, executable);
		createEnvCharPtrVector(env_c_vector, env_vector);

		if (execve(executable.c_str(), argv_vector.data(), env_c_vector.data()) == -1)
		{
			std::cerr << "Error: Execve failed: " << std::strerror(errno) << std::endl;
			closeTwoPipes(pipe_to_child_[READ], pipe_from_child_[WRITE]);
		}
		exit(1);
	}
	// addEventWithData(epoll_fd);
	// send data as write action to server EPOLLOUT instead of sendDataToStdin
	sendDataToStdin(post_data_);
	// Not wait for child but fork a timeout where also close the piples
	waitForChild();
	// not here, the server get this response
	getResponseFromCGI();
	// close all pipes in timeout mangager
	watchDog();
	closeAllPipes();
	// this check somewhere else
	if (!isNPHscript(executable))
		rewriteResonseFromCGI();
	// closeTwoPipes(pipe_to_child_[READ], pipe_from_child_[WRITE]);
}
CGI::~CGI(void) {}

void	CGI::create_events_from_pipes() {
	epoll_event_pipe_[0].data.fd = pipe_to_child_[WRITE];
	epoll_event_pipe_[0].events = EPOLLOUT;
	epoll_event_pipe_[1].data.fd = pipe_from_child_[READ];
	epoll_event_pipe_[1].events = EPOLLIN;
}

std::string CGI::getResponse(void) {
	return (response_);
}


#define TIMEOUT 10 // from configfile?

void	CGI::watchDog(void) {
	pid_t	pid;
	int		status;
	time_t	start;
	time_t	now;
	bool	timeout = false;

	pid = fork();
	if (pid < 0)
		throwException("Fork failed");
	else if (pid == 0)
	{
		start = time(NULL);
		while (waitpid(pid_, &status, WNOHANG) == 0)
		{
			now = time(NULL);
			if (now - start > TIMEOUT)
			{
				timeout = true;
				break ;
			}
			sleep(1);
		}
		if (timeout)
			kill(pid_, SIGKILL);
		closeAllPipes();
		exit(0);
	}
	else
	{
		closeAllPipes();
	}
}

/**
 * @brief extract statuscode from CGI response
 * @return int with statuscode or zero if not found
 */
int	CGI::getStatusCodeFromResponse(void) {
	std::regex	status_code_regex(R"(HTTP/1.1 (\d+))");
	std::smatch	match;
	int			status_code = 0;

	if (!response_.empty() && std::regex_search(response_, match, status_code_regex))
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
		if (waitpid(pid_, &status_, WNOHANG) > 0)
			return ;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cerr << "Timeout in CGI, process will be killed" << std::endl;
	if (kill(pid_, SIGKILL) == -1)
		std::cerr << "Error kill: " << std::strerror(errno) << std::endl;
}

/**
 * @brief checks response status from CGI and receives header (and body) from pipe
 * if statuscode is not set it wil generate a Internal Server Error
 */
void	CGI::getResponseFromCGI(void) {
	if (WIFEXITED(status_)) {
		int return_value = WEXITSTATUS(status_);
		response_ = receiveBuffer();
		if (return_value != 0) {
			int status_code = getStatusCodeFromResponse();
			std::cerr << "status_code; " << status_code << std::endl;
			// compare with configfile error pages.
			std::cerr << "response" << response_ << std::endl;
		}
	}
	else
		response_ = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
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
	
	if (std::regex_match(response_, match, r_status) && match.size() == 2)
		new_response += "HTTP/1.1 " + std::string(match[1]) + "\r\n";
	if (std::regex_match(response_, match, r_location) && match.size() == 2)
		new_response += "Location: " + std::string(match[1]) + "\r\n";
	if (std::regex_match(response_, match, r_content_type) && match.size() == 2)
		new_response += "Content-Type: " + std::string(match[1]) + "\r\n";
	if (new_response.empty())
	{
		std::cerr << "Error: Received wrong formated header from CGI" << std::endl;
		response_ = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		return ;
	}
	new_response += "\r\n";

	std::size_t	index = response_.find("\r\n\r");
	if (index != std::string::npos)
	{
		if (index + 4 < response_.size())
			new_response += response_.substr(index + 4) + "\r\n";
	}
	response_ = new_response;
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
	
	ssize_t bytesRead = read(pipe_from_child_[READ], buffer, sizeof(buffer) - 1);
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

	readBytes = write(pipe_to_child_[WRITE], post_data.c_str(), post_data.size());
	if (readBytes != (ssize_t)post_data.size())
	{
		if (readBytes == -1)
			std::cerr << "Error write: " << std::strerror(errno) << std::endl;
		else
			std::cerr << "Error write: not written right amount" << std::endl;
	}
	close(pipe_to_child_[WRITE]);
	pipe_to_child_[WRITE] = -1;
}

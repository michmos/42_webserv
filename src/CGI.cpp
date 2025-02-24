#include "CGI.hpp"

void	throw_exception(const char *msg) {
	std::perror(msg);
	throw std::exception();
}


void	throw_exception_exit(const char *msg) {
	std::perror(msg);
	exit(EXIT_FAILURE);
}

std::string CGI::get_response_CGI( void ) {
	return (m_response);
}

/**
 * @brief forks the process to execve the CGI, with POST sends buffer to child
 * @param executable const string CGI filename
 * @param env_vector char* vector with key-values as env argument to CGI
 */
CGI::CGI( const std::string &executable, std::vector<std::string> env_vector, const std::string &post_data)
{
	std::cout << std::flush;
	if (pipe(m_pipe_to_child) < 0 || pipe(m_pipe_from_child) < 0)
		throw_exception("Pipe failed");
	m_pid = fork();
	if (m_pid < 0)
		throw_exception("Fork failed");
	if (m_pid == 0) 
	{
		close(m_pipe_to_child[WRITE]);
		close(m_pipe_from_child[READ]);
		if (dup2(m_pipe_to_child[READ], STDIN_FILENO) < 0)
			throw_exception_exit("dub2 failed");
		if (dup2(m_pipe_from_child[1], STDOUT_FILENO) < 0)
			throw_exception_exit("dub2 failed");

		std::vector<char*>	argv_vector;
		argv_vector.push_back(const_cast<char *>(executable.c_str()));
		argv_vector.push_back(NULL);

		std::vector<char*> charPointerVector;
	    for (auto& str : env_vector)
	        charPointerVector.push_back(const_cast<char*>(str.c_str()));
	    charPointerVector.push_back(NULL);
		if (execve(executable.c_str(), argv_vector.data(), charPointerVector.data()) == -1)
		{	
			std::perror("execve failed");
			close(m_pipe_to_child[READ]);
			close(m_pipe_from_child[WRITE]);
		}
		exit(1);
	}
	close(m_pipe_to_child[READ]);
	close(m_pipe_from_child[WRITE]);
	send_body_to_stdin(post_data);
	wait(&m_status);
	m_response = receive_buffer();
}

CGI::~CGI(void) {}

/**
 * @brief receives a response from child with a headerfile and body to return to the client
 * @return string buffer with response from child or error msg that something went wrong
 */
std::string	CGI::receive_buffer(void) {
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

void	CGI::send_body_to_stdin( const std::string &post_data) {
	if (write(m_pipe_to_child[WRITE], post_data.c_str(), post_data.size()) != (ssize_t)post_data.size())
		std::perror("write failed");
	
	close(m_pipe_to_child[WRITE]);
}

/**
 * @brief checks if executable is allowed and valid
 * @param path string with path and filename
 * @return bool if cgi script is valid
 */
bool CGI::is_cgi_script(const std::string &path)
{
	std::string executable = get_script_executable(path);
	return !executable.empty();
}

/**
 * @brief compares executable extension with the allowed cgi scripts
 * @param path string with path and filename
 * @return string with path to executable program or empty when not correct
 */
std::string CGI::get_script_executable(const std::string &path)
{
	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return "/usr/bin/python3";
	if (path.size() >= 4 && path.substr(path.size() - 4) == ".php")
		return "/usr/bin/php";
	return "";
}

/**
 * @brief This info I need for the CGI, only QUERY_STRING I have to find out with the HTML script
 * also need to know where to store the files, specified in conf?
 */
// int main(void) {
// 	std::vector<char*> env_vector = {
// 			const_cast<char*>("CONTENT_LENGTH=163"),
// 			const_cast<char*>("REQUEST_TARGET=pictures.jpeg"),
// 			const_cast<char*>("CONTENT_TYPE=text"),
// 			const_cast<char*>("REQUEST_METHOD=POST"),
// 			const_cast<char*>("QUERY_STRING=filename=picture.jpeg"),
// 			const_cast<char*>("PATH_INFO=/upload"),
// 			nullptr // Zorg ervoor dat het eindigt met een null-pointer voor execve
// 	};
// 	std::string post_data = "----MyBoundary\r\nContent-Disposition: form-data; name=\"file\"; filename=\"example.jpg\"\r\n \
// 						Content-Type: image/jpeg\r\n\r\n<JPEG_IMAGE_DATA>\r\n----MyBoundary--\r\nlalal\n\n";
	
// 	CGI("upload_delete.py", env_vector, post_data);
// 	return (0);
// }
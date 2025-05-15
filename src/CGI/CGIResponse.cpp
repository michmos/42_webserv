#include "../../inc/CGI/CGI.hpp"
#include "../../inc/HTTP/HTTPClient.hpp"
#include "../../inc/Webserv/Logger.hpp"
#include <regex>

// #################     RESPONSE_FROM_CGI     ###################
// ###############################################################

static std::string	receiveBuffer(int fd) {
	char buffer[READSIZE + 1] = {'\0'};
	
	ssize_t bytesRead = read(fd, buffer, READSIZE);
	if (bytesRead == -1) {
		throw ClientException(std::string("CGI read(): ") + strerror(errno));
	}
	return (buffer);
}

/**
 * @brief checks response status from CGI and receives header (and body) from pipe
 * if statuscode is not set it wil generate a Internal Server Error
 */
void	CGI::getResponseFromCGI(const SharedFd &fd, uint32_t events) {
	if (fd.get() != pipes_[FROM_CGI_READ].get() || !(events & (EPOLLIN | EPOLLHUP)))
		return ;

	if (events & EPOLLERR) {
		throw ClientException("getResponseFromCGI(): received EPOLLERR on fd: " + std::to_string(fd.get()));
	}

	std::string buffer = receiveBuffer(fd.get());
	response_ += buffer;
	if (buffer.empty()) {
		delFromEpoll_cb_(pipes_[FROM_CGI_READ].get());
		pipes_[FROM_CGI_READ] = -1;
		CGI_STATE_ = HANDLE_RSPNS_CGI;
	}
}

bool	CGI::isCGIProcessFinished(void) {
	pid_t	result;

	if (finished_)
		return (true);

	result = waitpid(pid_, &status_, WNOHANG);
	if (result == pid_) {
		finished_ = true;
		return (true);
	}
	return (false);
}

bool	CGI::isCGIProcessSuccessful(void) {
	if (WIFEXITED(status_) && WEXITSTATUS(status_) == 0)
		return (true);
	return (false);
}

bool	CGI::timedOut(void) {
	if (timeout_)
		return (true);
	if (isCGIProcessFinished())
		return (false);

	time_t current_time = time(NULL);
	if (current_time - start_time_ > TIMEOUT) {
		timeout_ = true;
		return (true);
	}
	return (false);
}

/// @brief Set up a response for the client after receiving the header from the CGI
/// saves the result again in response_
void	CGI::rewriteResponseFromCGI(void) {
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
		Logger::getInstance().log(LOG_DEBUG, "Error: Received wrong formated header from CGI");
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
 * @brief extract statuscode from CGI response
 * @return int with statuscode or zero if not found
 */
int	CGI::getStatusCodeFromResponse(void) {
	std::regex	status_code_regex(R"(HTTP/1.1 (\d+))");
	std::smatch	match;
	int			status_code = 500;

	if (!response_.empty() && std::regex_search(response_, match, status_code_regex))
	{
		std::string to_string = match[1];
		if (to_string.size() < 9)
			status_code = std::stoi(match[1]);
	}
	else {
		Logger::getInstance().log(LOG_DEBUG, "Error: No response or statuscode is found in response");
	}
	return (status_code);
}

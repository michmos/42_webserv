#include "../../inc/HTTP/HTTPClient.hpp"
#include <sstream>

#define TERMINATOR_CHUNK "0\r\n\r\n"

std::string	HTTPClient::readFrom(int fd) {
	char	buff[READSIZE + 1] = { '\0'};
	int		bytes_read;

	bytes_read = recv(fd, buff, READSIZE, MSG_DONTWAIT);
	if (bytes_read == -1) {
		throw ClientException("Client: " + std::to_string(fd) + ": recv(): " + strerror(errno));
	} else if (bytes_read == 0) {
		throw ClientException("Client closed socket");
	}
	return (std::string(buff, bytes_read));
}

void	HTTPClient::writeToFd(const SharedFd &fd, const std::string &response) {
	ssize_t	bytes_write;

	bytes_write = send(fd.get(), response.c_str(), response.size(), MSG_DONTWAIT);
	if (bytes_write == -1)
		throw ClientException("Client: " + std::to_string(fd.get()) + ": send(): " + strerror(errno));
}

/**
 * @brief subtract the header from the response and add Transfer-Encoding to header
 * @return string with header + "Transfer-Encoding: chunked"
 */
std::string	HTTPClient::getHeaderInclTransferEncoding() {
	size_t		split;
	std::string	header;

	split = response_.find("\r\n\r\n");
	if (split == std::string::npos)
		return ("");
	header = response_.substr(0,split) + "\r\nTransfer-Encoding: chunked\r\n\r\n";
	response_.erase(0, split + 4);
	return (header);
}

/**
 * @brief subtract header if first_msg, else adds hexsize and makes chunk
 * @return string with chunk
 */
std::string	HTTPClient::extractChunk(void) {
	size_t				chunksize;
	std::ostringstream	chunk;

	if (first_response_)
		return (getHeaderInclTransferEncoding());

	chunksize = (WRITESIZE < response_.length()) ? WRITESIZE : response_.length();

	if (chunksize != 0)
	{
		chunk << std::hex << chunksize << "\r\n";
		chunk << response_.substr(0, chunksize) << "\r\n";
	}
	response_.erase(0,chunksize);
	if (response_.empty())
	{
		chunk << "0\r\n\r\n";
		STATE_ = DONE;
	}
	return (chunk.str());
}

/**
 * @brief writes response to Client (in chunks or not)
 * @param fd SharedFd from Client
 */
void	HTTPClient::writeToClient(const SharedFd &fd) {
	if (response_.length() > WRITESIZE || !first_response_) // IF CHUNKED
	{
		std::string chunk = extractChunk();
		writeToFd(fd, chunk);
	}
	else // ALL IN ONCE
	{
		writeToFd(fd, response_);
		response_.clear();
		STATE_ = DONE;
	}
	first_response_ = false;
	return ;
}

#include "../../inc/HTTP/HTTPClient.hpp"
#define TERMINATOR_CHUNK "0\r\n\r\n"

std::string	HTTPClient::readFrom(int fd) {
	char	buff[READSIZE + 1] = { '\0'};
	int		bytes_read;

	bytes_read = recv(fd, buff, READSIZE, MSG_DONTWAIT);
	if (bytes_read == -1)
		throw ClientException("Client: " + std::to_string(fd) + ": recv(): " + strerror(errno));
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
 * @param first_msg bool if first msg has to be send
 * @return string with chunk
 */
std::string	HTTPClient::getChunk(bool first_msg) {
	size_t				chunksize;
	std::ostringstream	chunk_os;
	std::string			chunk_response;

	if (first_msg)
		return (getHeaderInclTransferEncoding());

	chunksize = (WRITESIZE < response_.length()) ? WRITESIZE : response_.length();

	chunk_os << std::hex << chunksize << "\r\n";
	chunk_os << response_.substr(0, chunksize) << "\r\n";
	chunk_response = std::move(chunk_os.str());
	response_.erase(0,chunksize);
	return (chunk_response);
}

/**
 * @brief writes response to Client (in chunks or not)
 * @param fd SharedFd from Client
 * @param send_first_msg bool
 */
void	HTTPClient::writeToClient(const SharedFd &fd, bool send_first_msg) {
	if (send_first_msg) // ONLY SEND HEADER
	{
		if (message_que_.empty())
			return ;
		response_ = message_que_.front();
		message_que_.erase(message_que_.begin());
		if (response_.empty())
			return ;
	}
	if (response_.length() > WRITESIZE || !send_first_msg) // IF CHUNKED
	{
		std::string write_msg = getChunk(send_first_msg);
		if (write_msg == TERMINATOR_CHUNK)
			STATE_ = DONE;
		writeToFd(fd, write_msg);
	}
	else // ALL IN ONCE
	{
		writeToFd(fd, response_);
		response_.clear();
		STATE_ = DONE;
	}
	return ;
}

#include "../../inc/HTTP/HTTPClient.hpp"

std::string	HTTPClient::readFrom(int fd) {
	char	buff[READSIZE + 1] = { '\0'};
	int		bytes_read;

	bytes_read = read(fd, buff, READSIZE);
	if (bytes_read == -1)
		return ("");
	return (std::string(buff, bytes_read));
}

ssize_t	HTTPClient::writeToFd(const SharedFd &fd, const std::string &response) {
	ssize_t	bytes_write;

	bytes_write = write(fd.get(), response.c_str(), response.size());
	if (bytes_write == -1)
		throw std::runtime_error("write(): " + std::string(strerror(errno)));
	else if (bytes_write != (ssize_t)response.size())
		; // TO DO ... not really sure what to do here
	return (bytes_write); // not needed to return
}

/**
 * @brief subtract the header from the response and add Transfer-Encoding to it
 * @return string with header + chunked
 */
std::string	HTTPClient::getHeaderInclChunked() {
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
		return (getHeaderInclChunked());

	chunksize = response_.length();
	if (chunksize > CHUNKSIZE)
		chunksize = CHUNKSIZE;

	chunk_os << std::hex << chunksize << "\r\n";
	chunk_os << response_.substr(0, chunksize) << "\r\n";
	chunk_response = chunk_os.str();
	response_.erase(0,chunksize);
	return (chunk_response);
}

/**
 * @brief writes response to Client (in chunks or not)
 * @param fd SharedFd from Client
 * @param send_first_msg bool
 */
void	HTTPClient::writeToClient(const SharedFd &fd, bool send_first_msg) {
	static bool	isNPHscript = false;
	
	if (send_first_msg) // ONLY SEND HEADER
	{
		if (message_que_.empty())
			return ;
		response_ = message_que_.front();
		message_que_.erase(message_que_.begin());
		if (response_.empty())
			return ;
		if (isCgiRequ_)
			isNPHscript = cgi_->isNPHscript();
	}
	if (!isNPHscript && (response_.length() > WRITESIZE || !send_first_msg)) // IF CHUNKED
	{
		std::string write_msg = getChunk(send_first_msg);
		if (write_msg == "0\r\n\r\n")
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

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
	return (bytes_write);
}

std::string	getHeaderInclChunked(std::string &response) {
	size_t		split;
	std::string	header;

	split = response.find("\r\n\r\n");
	if (split == std::string::npos)
		return (""); //?
	header = response.substr(0,split) + "\r\nTransfer-Encoding: chunked\r\n\r\n";
	response.erase(0, split + 4);
	std::cerr << "header: " << header << std::endl;
	return (header);
}

std::string	getChunk(std::string &response, bool first_msg) {
	size_t				chunksize;
	std::ostringstream	chunk_os;
	std::string			chunk_response;

	if (first_msg)
		return (getHeaderInclChunked(response));

	chunksize = response.length();
	if (chunksize > CHUNKSIZE)
		chunksize = CHUNKSIZE;

	chunk_os << std::hex << chunksize << "\r\n";
	chunk_os << response.substr(0, chunksize) << "\r\n";
	chunk_response = chunk_os.str();
	response.erase(0,chunksize);
	if (response.empty() || response.size() <= 5) // REMAINING ONLY \r\n\r\n
	{
		response.clear();
		return("0\r\n\r\n");
	}
	return (chunk_response);
}

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
		std::string write_msg = getChunk(response_, send_first_msg);
		if (write_msg.size() <= 10)
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


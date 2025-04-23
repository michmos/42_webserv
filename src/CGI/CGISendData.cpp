#include "../../inc/CGI/CGI.hpp"
#include "../../inc/HTTP/HTTPClient.hpp"

// ###############################################################
// ###################### SEND_TO_CGI ############################

/**
 * @brief writes body to stdin for CGI and closes write end pipe
 * @param post_data string with body
 */
void	CGI::sendDataToCGI( const SharedFd &fd, uint32_t events ) {
	ssize_t				write_bytes;

	if (fd.get() != pipes_[TO_CGI_WRITE].get() || !(events & EPOLLOUT))
		return ;

	if (events & (EPOLLHUP | EPOLLERR)) {
		throw ClientException("sendDataToCGI(): received EPOLLHUP or EPOLLERR on fd: " + std::to_string(fd.get()));
	}

	if (!request_.body.empty())
	{
		if (send_data_.empty())
			send_data_ = request_.body;
		size_t writesize = (send_data_.size() < WRITESIZE) ? send_data_.size() : WRITESIZE;

		write_bytes = write(fd.get(), send_data_.c_str(), writesize);
		if (write_bytes == -1) {
			throw ClientException("CGI write(): " + std::to_string(fd.get()) + " : " + strerror(errno));
		} else if (write_bytes < (ssize_t)send_data_.size()) {
			send_data_.erase(0, write_bytes);
			return ;
		}
	}
	delFromEpoll_cb_(pipes_[TO_CGI_WRITE].get());
	pipes_[TO_CGI_WRITE] = -1;
	CGI_STATE_ = RCV_FROM_CGI;
}

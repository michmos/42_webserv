
#include "CGIPipes.hpp"

CGIPipes::CGIPipes(void) {

}

CGIPipes::~CGIPipes(void) {
	closeAllPipes();
}

void	CGIPipes::setCallbackFunction(std::function<void(int, int)> callback) {
	pipe_callback_ = callback;
}

std::vector<int>&	CGIPipes::getLastPipes(void) { return (pipes_.back()); }

/// @brief set pipes to -1 and then open both (to and from child) + nonblock
void	CGIPipes::addNewPipes(void) {
	std::vector<int>	new_pipes(4, -1);
	int					pipe_from_child[2];
	int					pipe_to_child[2];

	std::memset(pipe_from_child, -1, sizeof(pipe_from_child));
	std::memset(pipe_to_child, -1, sizeof(pipe_to_child));
	if (pipe(pipe_to_child) < 0)
	{
		std::perror("pipe_from_child failed");
		throw std::exception();
	}
	if (pipe(pipe_from_child) < 0)
	{
		std::perror("pipe_to_child failed");
		close(pipe_to_child[WRITE]);
		close(pipe_to_child[READ]);
		throw std::exception();
	}
	fcntl(pipe_to_child[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(pipe_to_child[READ], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_child[WRITE], F_SETFL, O_NONBLOCK);
	fcntl(pipe_from_child[READ], F_SETFL, O_NONBLOCK);
	new_pipes[0] = pipe_from_child[READ];
	new_pipes[1] = pipe_from_child[WRITE];
	new_pipes[2] = pipe_to_child[READ];
	new_pipes[3] = pipe_to_child[WRITE];
	pipes_.push_back(new_pipes);
	addPipesToEpoll();
}

void	CGIPipes::addPipesToEpoll() {
	std::vector<int>	pipes = pipes_.back();

	pipe_callback_(pipes[TO_CHILD_WRITE], pipes[FROM_CHILD_READ]);
	// ep_.add(pipes[TO_CHILD_WRITE], EPOLLOUT | EPOLLET);
	// ep_.add(pipes[FROM_CHILD_READ], EPOLLIN | EPOLLET);
}

/// @brief closes all pipes that are stored in a vector<vector<int>> array
void	CGIPipes::closeAllPipes(void) {
	for (size_t i = 0; i < pipes_.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (pipes_[i][j] != -1)
			{
				close(pipes_[i][j]);
				pipes_[i][j] = -1;
			}
		}
	}
}

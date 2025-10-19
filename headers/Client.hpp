#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
# include "HTTP.hpp"
// # include <chrono>

enum ClientState {
	RD_HEADER,
	RD_BODY,
	WRITING
};

class Client {

	public:
		Client(void);
		~Client(void);

		int	fd;

		// ClientState state;
		// int socker_fd;
		// std::chrono::time_point<std::chrono::system_clock> ts;

		// std::string read_buff;
		// size_t rdi;
		// std::string write_buff;
		// size_t wdi;

};

#endif // CLIENT_HPP

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
// # include <chrono>

class Server;

enum ClientState {
	RD_HEADER,
	RD_BODY,
	WRITING
};

class Client {

	public:
		Client(void);
		~Client(void);

		int			fd;
		std::string	out_buffer;


		bool		isCGI;
		int			referringFD;
		pid_t		CGIpid;

		void	setCGI(int cgiFD, Server& server);
		Response   response;
		Request    request;
		Parser	   parser;

		// ClientState state;
		// int socker_fd;
		// std::chrono::time_point<std::chrono::system_clock> ts;

		// std::string read_buff;
		// size_t rdi;
		// std::string write_buff;
		// size_t wdi;

};

#endif // CLIENT_HPP

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
};

#endif // CLIENT_HPP

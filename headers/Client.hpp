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
		Client(const Client& Client);
		~Client(void);

		Client&	operator=(const Client& Client);

		int			fd;
		std::string	out_buffer;
		Request		request;
		Response	response;
		Parser		parser;

		bool		isCGI;
		int			referringFD;
		pid_t		CGIpid;
		void		setCGI(int cgiFD, Server& server);

};

#endif // CLIENT_HPP

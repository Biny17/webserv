#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"
// # include <chrono>

class Server;
class Parser;
class Response;

class Client {
	public:
		Client(Server &s);
		Client(const Client& Client);
		~Client(void);

		Server&		server;
		int			fd;
		int			epollStatus;
		Request		request;
		Response	response;
		Parser		parser;
		bool		isCGI;
		int			referringFD;
		pid_t		CGIpid;
		void		setCGI(int referringFD);

		std::string	cat;
		bool		changedCat;
		void		switchCat(void);

		void 		RequestHandler();
};

#endif // CLIENT_HPP

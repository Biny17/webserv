#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

class Client;

class Server {

	public:
		Server(void);
		~Server(void);

		std::string	name;
		int			port;
		int			socket;

		std::map<int, Client>	clients;

		bool	hasFD(int fd) const;
		void	addClient(int clifd);
		void	removeClient(int clifd);

};

#endif // SERVER_HPP

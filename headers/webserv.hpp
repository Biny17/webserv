#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <map>
# include <vector>
# include <stdint.h>
# include "network.hpp"

struct Client {
	int fd;
	Client(void){}
	Client(int fd){this->fd = fd;}
	~Client(void){close(this->fd);}
};

struct Server {
	Server(void){}
	~Server(){}
	std::string name;
	std::map<int, Client> clients;
	int		port;
	int		socket;
	bool	has_fd(int fd) const
	{
		(void)fd;
		if (fd == this->socket)
			return (true);
		std::map<int, Client>::const_iterator it;
		std::map<int, Client>::const_iterator ite = this->clients.end();
		for (it = this->clients.begin(); it != ite; it++)
		{
			if (it->first == fd)
				return (true);
		}
		return (false);
	}
	void	add_client(int clifd)
	{
		this->clients[clifd] = Client(clifd);
	}
	void	remove_client(int clifd)
	{
		this->clients.erase(clifd);
	}
};

#endif // WEBSERV_HPP
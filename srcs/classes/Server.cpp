#include "Server.hpp"

Server::Server(void) {}

Server::~Server(void) {}

bool	Server::hasFD(int fd) const
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

void	Server::addClient(int clifd)
{
	this->clients[clifd] = Client(clifd);
}

void	Server::removeClient(int clifd)
{
	this->clients.erase(clifd);
}
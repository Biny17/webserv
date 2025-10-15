#include "Server.hpp"

Server::Server(void)
{
	this->port = 8080;
	this->socket = -1;
}

Server::~Server(void)
{
	if (this->socket >= 0)
		close(this->socket);
}

// Search for the FD in the server (including himself)
bool	Server::hasFD(int fd) const
{
	if (fd == this->socket)
		return (true);

	if (this->clients.find(fd) != this->clients.end())
		return (true);
	return (false);
}

// Add a client to the server's client map
void	Server::addClient(int clifd)
{
	this->clients[clifd] = Client();
	this->clients[clifd].fd = clifd;
}

// Remove the client form the server's client map
void	Server::removeClient(int clifd)
{
	this->clients.erase(clifd);
}

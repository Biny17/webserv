#include "Server.hpp"

Server::Server(void)
{
	this->max_upload = 0;
}

Server::~Server(void)
{
	std::vector<int>::iterator	it;
	std::vector<int>::iterator	ite = this->sockets.end();
	for (it = this->sockets.begin(); it != ite; ++it)
	{
		if (*it >= 0)
			close(*it);
	}
}

bool	Server::isSockFD(int fd) const
{
	if (std::find(this->sockets.begin(), this->sockets.end(), fd) != this->sockets.end())
		return (true);
	return (false);
}

// Search for the FD in the server (including himself)
bool	Server::hasFD(int fd) const
{
	if (this->isSockFD(fd))
		return (true);

	if (this->clients.find(fd) != this->clients.end())
		return (true);
	return (false);
}

// Add a client to the server's client map
void	Server::addClient(int clifd)
{
	//make pair
	this->clients.insert(std::make_pair(clifd, Client(*this, clifd)));
}

// Remove the client form the server's client map
void	Server::removeClient(int clifd)
{
	this->clients.erase(clifd);
}

std::ostream	&operator<<(std::ostream &o, Server const &serv) {
	o << "---------------------------------server----------------------------------" << std::endl;
	if (!serv.listen.empty())
	{
		o << "listen : ";
		for (std::vector<int>::const_iterator it = serv.listen.begin(); it != serv.listen.end(); ++it)
			o << *it << " ";
		o << std::endl;
	}
	if (!serv.server_name.empty())
		o << "server_name : " << serv.server_name << std::endl;
	o << "max_upload : " << serv.max_upload << std::endl;
	if (!serv.index_page.empty())
	{
		o << "index : ";
		for (std::vector<std::string>::const_iterator it = serv.index_page.begin(); it != serv.index_page.end(); ++it)
			o << *it << " ";
		o << std::endl;
	}
	if (!serv.err_page.empty())
		o << "error_page amount : " << serv.err_page.size() << std::endl;
	if (!serv.locations.empty())
	{
		o << "locations amount : " << serv.locations.size() << std::endl;
		std::vector<Location>::const_iterator	it = serv.locations.begin();
		for (it = serv.locations.begin(); it != serv.locations.end(); ++it)
		{
			std::cout << *it << std::endl;
		}
	}
	o << "-------------------------------------------------------------------------" << std::endl;
	return o;
}

#include "Client.hpp"

Client::Client(void)
{
	this->fd = -1;
	this->isCGI = false;
	this->cgi_body = "";
}

Client::~Client(void)
{
	if (this->fd >= 0)
		close(this->fd);
}

// Define the client as a CGI and add it to epoll
void	Client::setCGI(int referringFD, Server& server)
{
	this->isCGI = true;
	this->referringFD = referringFD;

	struct epoll_event cli_event;
	cli_event.events = EPOLLIN;
	cli_event.data.fd = this->fd;
	if (epoll_ctl(server.epfd, EPOLL_CTL_ADD, this->fd, &cli_event) == -1)
	{
		std::cout << "Couldn't add the cgi to epoll" << std::endl;
		server.removeClient(this->fd);
	}
}

#include "Client.hpp"

Client::Client(void)
	:parser(request, response)
{
	this->fd = -1;
	this->out_buffer = "";
	this->isCGI = false;
	this->CGIpid = -1;
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

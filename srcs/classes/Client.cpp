#include "Client.hpp"

Client::Client(void)
	:parser(this->request, this->response)
{
	this->fd = -1;
	this->out_buffer = "";

	this->isCGI = false;
	this->CGIpid = -1;
}

Client::Client(const Client& Client): parser(Client.parser)
{
	*this = Client;
}

Client::~Client(void)
{
	if (this->fd >= 0)
		close(this->fd);
}

Client&	Client::operator=(const Client& Client)
{
	if (this == &Client)
		return (*this);
	this->fd = Client.fd;
	this->out_buffer = Client.out_buffer;
	this->isCGI = Client.isCGI;
	this->referringFD = Client.referringFD;
	this->CGIpid = Client.CGIpid;
	this->request = Client.request;
	this->response = Client.response;
	this->parser = Client.parser;
	return (*this);
}

// Define the client as a CGI and add it to epoll
void	Client::setCGI(int referringFD, Server& server)
{
	this->isCGI = true;
	this->referringFD = referringFD;

	struct epoll_event cli_event;
	cli_event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;;
	cli_event.data.fd = this->fd;
	if (epoll_ctl(server.epfd, EPOLL_CTL_ADD, this->fd, &cli_event) == -1)
	{
		std::cout << "Couldn't add the cgi to epoll" << std::endl;
		server.removeClient(this->fd);
	}
}

#include "Client.hpp"

Client::Client(Server &s)
	:server(s), response(*this), parser(this->request, this->response)
{
	this->fd = -1;

	this->isCGI = false;
	this->CGIpid = -1;
}

Client::Client(const Client& other)
	: server(other.server), fd(other.fd), response(*this), parser(this->request, this->response)
{
	this->isCGI = other.isCGI;
	this->CGIpid = other.CGIpid;
	this->referringFD = other.referringFD;
}

Client::~Client(void)
{
	if (this->fd >= 0)
		close(this->fd);
}

// Define the client as a CGI and add it to epoll
void	Client::setCGI(int referringFD)
{
	this->isCGI = true;
	this->referringFD = referringFD;

	struct epoll_event cli_event;
	cli_event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	cli_event.data.fd = this->fd;
	this->epollStatus = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	if (epoll_ctl(this->server.epfd, EPOLL_CTL_ADD, this->fd, &cli_event) == -1)
	{
		std::cout << "Couldn't add the cgi to epoll" << std::endl;
		this->server.removeClient(this->fd);
	}
}

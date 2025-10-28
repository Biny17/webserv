#include "Client.hpp"

Client::Client(Server &s)
	:server(s), response(*this), parser(this->request, this->response)
{
	this->fd = -1;

	this->isCGI = false;
	this->CGIpid = -1;
	this->cat = "mouli1";
	this->changedCat = false;
}

Client::Client(const Client& other)
	: server(other.server), fd(other.fd), response(*this), parser(this->request, this->response)
{
	this->isCGI = other.isCGI;
	this->CGIpid = other.CGIpid;
	this->referringFD = other.referringFD;
	this->cat = other.cat;
	this->changedCat = other.changedCat;
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

void	Client::switchCat(void)
{
	this->changedCat = true;
	if (this->request.headers.find("Cookie") != this->request.headers.end())
		this->cat = this->request.headers["Cookie"].substr(7, 13);
	this->cat = this->cat == "mouli1" ? "mouli2" : "mouli1";
}

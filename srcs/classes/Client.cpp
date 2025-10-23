#include "Client.hpp"

Client::Client():parser(this->request, this->response) {}

Client::Client(Server *s)
	:server(s), parser(this->request, this->response)
{
	this->fd = -1;
	this->out_buffer = "";

	this->isCGI = false;
	this->CGIpid = -1;
}

Client::Client(const Client& Client): server(Client.server), parser(Client.parser)
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
	this->server = Client.server;
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
	cli_event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	cli_event.data.fd = this->fd;
	if (epoll_ctl(server.epfd, EPOLL_CTL_ADD, this->fd, &cli_event) == -1)
	{
		std::cout << "Couldn't add the cgi to epoll" << std::endl;
		server.removeClient(this->fd);
	}
}

size_t n_prefix_match(std::string &target, std::string &location)
{
	size_t n = 0;
	while (target[n] == location[n]
			&& n < target.length()
			&& n < location.length())
		n++;
	if (n == location.length())
		return n;
	return 0;
}

Location& findLocation(Request& req, Server& serv)
{
	Location& ret = serv.locations[0];
	size_t biggest = 1;

	size_t i = 0;
	while(i < serv.locations.size()) {
		size_t tmp = n_prefix_match(req.path, (serv.locations[i]).path);
		std::cout << req.path << " " << (serv.locations[i]).path << "  " << tmp << std::endl;
		if (tmp > biggest) {
			biggest = tmp;
			ret = serv.locations[i];
		}
		i++;
	}
	return ret;
}

void Client::RequestHandler()
{
	if (server == nullptr) {
		response.error_code = 500;
		response.msg = "";
		return ;
	}
	Location& loc = findLocation(request, *server);
	std::cout << "location matched: " << loc.path << std::endl << std::endl;
}
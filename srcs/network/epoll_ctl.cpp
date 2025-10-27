#include "webserv.hpp"

// Say the client to stop sending events because it is already bad
void	set_epoll_event(int epfd, Client& client, uint32_t event)
{
	struct epoll_event cli_event;
	cli_event.events = event | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	cli_event.data.fd = client.fd;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client.fd, &cli_event);
	client.epollStatus = event | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
}

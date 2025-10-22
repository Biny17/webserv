#include "webserv.hpp"

// Say the client to stop sending events because it is already bad
void	set_epoll_event(int epfd, int clifd, uint32_t event)
{
	struct epoll_event cli_event;
	cli_event.events = event | EPOLLHUP | EPOLLERR | EPOLLRDHUP;;
	cli_event.data.fd = clifd;
	epoll_ctl(epfd, EPOLL_CTL_MOD, clifd, &cli_event);
}

// Clear the client and remove it from epoll
void	disconnect_client(int epfd, int clifd, Server& server)
{
	server.removeClient(clifd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, clifd, NULL);
}

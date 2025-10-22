#include "webserv.hpp"

// Return the fd of the server where the client fd is assigned to
Server&	fetch_server(std::vector<Server>& servers, int fd)
{
	std::vector<Server>::iterator	it;
	std::vector<Server>::iterator	ite = servers.end();

	for (it = servers.begin(); it != ite; it++)
	{
		if (it->hasFD(fd) == true)
			return (*it);
	}

	return (servers[0]);
}

// Handle EPOLLERR events
void	handle_epollerr(struct epoll_event& event, int epfd)
{
	int		fd = event.data.fd;
	Server&	server_request = fetch_server(servers, fd);

	disconnect_client(epfd, fd, server_request);
}

// Handle EPOLLIN events
void	handle_epollin(struct epoll_event& event, int epfd)
{
	int		fd = event.data.fd;							// Get the fd of the socket of the event
	Server&	server_request = fetch_server(servers, fd);	// Fetch the server of the client

	if (server_request.isSockFD(fd))					// New client
		accept_new_client(epfd, fd, server_request);
	else												// Received form existing client
	{
		if (server_request.clients[fd].isCGI == true)	// The client is a CGI
			listen_cgi(server_request, server_request.clients[fd]);
		else
			read_client_data(epfd, fd, server_request);	// Process the request
	}
}

// Handle EPOLLOUT events
void	handle_epollout(struct epoll_event& event, int epfd)
{
	int		fd = event.data.fd;
	Server&	server_request = fetch_server(servers, fd);	// Fetch the server of the client

	if (send_response(fd, server_request.clients[fd].out_buffer) == true)
		set_epoll_event(epfd, fd, EPOLLIN);
}

// Event loop of epoll
void	event_loop(int epfd)
{
	struct epoll_event	events[MAX_EVENTS];

	while (!shutdown_serv)
	{
		// Search for events from tracked sockets
		int event_amount = epoll_wait(epfd, events, MAX_EVENTS, -1); // Get events (blocking)
		if (event_amount == -1)
			break;

		// Iterate over events
		for (int i = 0; i < event_amount; i++)
		{
			if (events[i].events & EPOLLERR)
				handle_epollerr(events[i], epfd);
			else if (events[i].events & (EPOLLIN | EPOLLHUP | EPOLLRDHUP))
				handle_epollin(events[i], epfd);
			else if (events[i].events & EPOLLOUT)
				handle_epollout(events[i], epfd);
		}
	}

	if (!shutdown_serv)
		perror("epoll_wait");
}

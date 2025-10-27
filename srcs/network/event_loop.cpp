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
void	handle_epollerr(struct epoll_event& event)
{
	int		fd = event.data.fd;							// Get the fd of the socket of the event
	Server&	server_request = fetch_server(servers, fd);	// Fetch the server of the client

	server_request.removeClient(fd);
}

// Handle EPOLLIN events
void	handle_epollin(struct epoll_event& event)
{
	int		fd = event.data.fd;							// Get the fd of the socket of the event
	Server&	server_request = fetch_server(servers, fd);	// Fetch the server of the client

	if (server_request.isSockFD(fd))					// New client
		accept_new_client(fd, server_request);
	else												// Received from existing client
	{
		std::map<int, Client>::iterator client_it = server_request.clients.find(fd);
		if (client_it != server_request.clients.end())
		{
			if (client_it->second.isCGI == true)		// The client is a CGI
				listen_cgi(server_request, client_it->second);
			else
				read_client_data(client_it->second, server_request);	// Process the request
		}
	}
}

// Handle EPOLLOUT events
void	handle_epollout(struct epoll_event& event)
{
	int		fd = event.data.fd;
	Server&	server_request = fetch_server(servers, fd);	// Fetch the server of the client

	std::map<int, Client>::iterator client_it = server_request.clients.find(fd);
	if (client_it != server_request.clients.end())
		client_it->second.response.Send();
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
		std::cout << event_amount << std::endl;
		for (int i = 0; i < event_amount; i++)
		{
			if (events[i].events & EPOLLERR)
				handle_epollerr(events[i]);
			else if (events[i].events & (EPOLLIN | EPOLLHUP | EPOLLRDHUP))
				handle_epollin(events[i]);
			else if (events[i].events & EPOLLOUT)
				handle_epollout(events[i]);
		}
	}

	if (!shutdown_serv)
		perror("epoll_wait");
}

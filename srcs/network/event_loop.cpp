#include "webserv.hpp"

void	try_timeouts(void)
{
	for (std::vector<Server>::iterator servIt = servers.begin(); servIt != servers.end(); ++servIt)
	{
		std::map<int, Client>::iterator cliIt = servIt->clients.begin();
		while (cliIt != servIt->clients.end())
		{
			if (cliIt->second.timeout.Check(cliIt->second.isCGI) == true)
			{
				std::cout << "A client has been timeout on server " << servIt->server_name << std::endl;
				Client&	client = cliIt->second;
				++cliIt;
				if (client.isCGI)
					kill_cgi(client);
				servIt->removeClient(client.fd);
			}
			else
				++cliIt;
		}
	}
}

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

	return (servers.front());
}

// Handle EPOLLERR events
void	handle_epollerr(struct epoll_event& event)
{
	int		fd = event.data.fd;					// Get the fd of the socket of the event
	Server&	server = fetch_server(servers, fd);	// Fetch the server of the client

	server.removeClient(fd);
}

// Handle EPOLLIN events
void	handle_epollin(struct epoll_event& event)
{
	int		fd = event.data.fd;						// Get the fd of the socket of the event
	Server&	server = fetch_server(servers, fd);		// Fetch the server of the client

	if (server.isSockFD(fd))						// New client
		accept_new_client(fd, server);
	else											// Received from existing client
	{
		std::map<int, Client>::iterator client_it = server.clients.find(fd);
		if (client_it != server.clients.end())
		{
			Client&	client = client_it->second;
			if (client.timeout.Enabled() == false)
				client.timeout.Start();
			if (client.isCGI == true)				// The client is a CGI
				listen_cgi(server, client);
			else
				read_client_data(client, server);	// Process the request
		}
	}
}

// Handle EPOLLOUT events
void	handle_epollout(struct epoll_event& event)
{
	int		fd = event.data.fd;
	Server&	server = fetch_server(servers, fd);	// Fetch the server of the client

	std::map<int, Client>::iterator client_it = server.clients.find(fd);
	if (client_it != server.clients.end())
		client_it->second.response.Send();
}

// Event loop of epoll
void	event_loop(int epfd)
{
	struct epoll_event	events[MAX_EVENTS];

	while (!shutdown_serv)
	{
		// Search for events from tracked sockets
		int event_amount = epoll_wait(epfd, events, MAX_EVENTS, EPOLL_TIMEOUT); // Get events (blocking)
		if (event_amount == -1)
			break;

		// Iterate over events
		for (int i = 0; i < event_amount; i++)
		{
			if (events[i].events & EPOLLERR)
				handle_epollerr(events[i]);
			else if (events[i].events & (EPOLLIN | EPOLLHUP | EPOLLRDHUP))
				handle_epollin(events[i]);
			else if (events[i].events & EPOLLOUT)
				handle_epollout(events[i]);
		}

		// Check timeouts of all clients on all servers and close them if necessary
		try_timeouts();
	}

	if (!shutdown_serv)
		perror("epoll_wait");
}

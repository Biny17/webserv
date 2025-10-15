#include "webserv.hpp"

// Return the fd of the server where the client fd is assigned to
Server&	fetch_server(std::vector<Server>& servers, int fd)
{
	std::vector<Server>::iterator it;
	std::vector<Server>::iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (it->hasFD(fd) == true)
			return (*it);
	}
	return (servers[0]);
}

// Event loop of epoll
void	event_loop(int epfd, std::vector<Server>& servers)
{
	struct epoll_event	events[MAX_EVENTS];

	while (!shutdown_serv)
	{
		// Search tracked socket events
		int event_amount = epoll_wait(epfd, events, MAX_EVENTS, -1);	// Get events (blocking)
		if (event_amount == -1)
			break;

		// Iterate over events
		for (int i = 0; i < event_amount; i++)
		{
			int		fd = events[i].data.fd;								// Get the fd of the socket of the event
			Server&	server_request = fetch_server(servers, fd);			// Fetch the server of the client

			if (fd == server_request.socket)							// New client
				accept_new_client(epfd, server_request);
			else														// Received form existing client
				read_client_data(epfd, fd, server_request);
		}
	}
	if (!shutdown_serv)
		perror("epoll_wait");

	// Close clients on exit
}
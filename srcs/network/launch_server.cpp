#include "webserv.hpp"

// Creation and initialisation of the epoll instance to track new clients
int	init_epoll(std::vector<Server>& servers)
{
	// Create epoll instance
	int epfd = epoll_create(1);
	if (epfd == -1)
	{
		perror("epoll_create");
		return (-1);
	}

	// Iterate over servers to track new clients on their socket
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		struct epoll_event event;
		event.events = EPOLLIN;		// Set for reading
		event.data.fd = it->socket;	// Attach server socket to epoll
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, it->socket, &event) == -1)
		{
			perror("epoll_ctl");
			std::cout << "An error occured while assigning the server `" << it->name << "` to epoll" << std::endl;
			servers.erase(it);
		}
	}

	// If all servers have failed to connect to epoll
	if (servers.empty())
	{
		close(epfd);
		return (-1);
	}

	return (epfd);
}

// Create a socket, bind it and put it in passive mode
bool	init_server(Server& server)
{
	// Create IPv4 TCP socket
	server.socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server.socket == -1)
	{
		perror("socket");
		return (false);
	}

	// Bind socket to port
	struct sockaddr_in addr;			// Address representation
	addr.sin_family = AF_INET;			// IPv4 addr
	addr.sin_port = htons(server.port);	// Set the port to listen on (htons convert to network endian)
	addr.sin_addr.s_addr = INADDR_ANY;	// Listen on all private network IPs
	if (bind(server.socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		return (false);
	}

	// Put socket in passive mode (ready to accept connections)
	if (listen(server.socket, SOMAXCONN) == -1)
	{
		perror("listen");
		return (false);
	}
	return (true);
}

// Setup the servers and call the event loop
void	launch_server(std::vector<Server>& servers)
{
	// Initialise all servers
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (init_server(*it) == false)
		{
			std::cout << "An error occured while creating the server `" << it->name << "`" << std::endl;
			servers.erase(it);
		}
	}

	// Quit if all servers have failed their initialisation
	if (servers.empty())
		return ;

	// Create epoll instance and initialise it
	int epfd = init_epoll(servers);
	if (epfd == -1)
		return ;

	// Set the signal to stop the server
	signal(SIGINT, &handle_shutdown);

	// Start the epoll event loop (wait for connections)
	event_loop(epfd, servers);

	// Close when the server stop
	close(epfd);
}

#include "webserv.hpp"

// Attach each server's sockets to epoll and set them on reading
bool	attach_sockets(int epfd, Server& server)
{
	size_t	failed_amount = 0;
	size_t	sockets_size = server.sockets.size();

	// Set socket on reading
	struct epoll_event event;
	event.events = EPOLLIN;

	// Iterate over each sockets of each server
	std::vector<int>::iterator it = server.sockets.begin();
	while (it != server.sockets.end())
	{
		event.data.fd = *it;

		(void)epfd;

		// Attach to epoll
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, *it, &event) == -1)
		{
			std::cout << "An error occured while assigning a port of the server `" << server.server_name << "` to epoll: " << strerror(errno) << std::endl;
			close(*it);
			it = server.sockets.erase(it);
			failed_amount++;
		}
		else
			++it;
	}

	if (failed_amount == sockets_size)
		return (false);
	return (true);
}

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
	std::vector<Server>::iterator it = servers.begin();
	while (it != servers.end())
	{
		it->epfd = epfd;
		if (attach_sockets(epfd, *it) == false)
		{
			std::cout << "Could not assign server `" << it->server_name << "` to epoll" << std::endl;
			it = servers.erase(it);
		}
		else
			++it;
	}

	// If all servers have failed to connect to epoll
	if (servers.empty())
	{
		close(epfd);
		return (-1);
	}

	return (epfd);
}

// Setup the port and bind it on the socket
bool	bind_port(int sockfd, int port)
{
	// Reset port to be usable again
	int	opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return (false);

	struct sockaddr_in addr;			// Address representation
	addr.sin_family = AF_INET;			// IPv4 addr
	addr.sin_addr.s_addr = INADDR_ANY;	// Listen on all private network IPs
	addr.sin_port = htons(port);			// Set the port to listen on (htons convert to network endian)

	// Bind ports to socket
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		return (false);

	return (true);
}

// Create the sockets, bind them and put them in passive mode
bool	init_sockets(Server& server)
{
	size_t	failed_amount = 0;

	// Iterate over ports
	std::vector<int>::iterator	it;
	std::vector<int>::iterator	ite = server.listen.end();
	for (it = server.listen.begin(); it != ite; ++it)
	{
		// Create IPv4 TCP socket
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			std::cout << "Failed to create socket for the port " << *it << " on server `" << server.server_name << "`: " << strerror(errno) << std::endl;
			failed_amount++;
			continue;
		}

		server.sockets.push_back(sockfd);	// Add socket to server socket list

		// Setup port and bind it to the socket
		if (bind_port(sockfd, *it) == false)
		{
			std::cout << "Failed to bind port " << *it << " to server `" << server.server_name << "`: " << strerror(errno) << std::endl;
			close(server.sockets.back());
			server.sockets.pop_back();
			failed_amount++;
			continue;
		}

		// Put socket in passive mode (ready to accept connections)
		if (listen(sockfd, SOMAXCONN) == -1)
		{
			std::cout << "Failed set socket on listening for the port " << *it << " on server `" << server.server_name << "`: " << strerror(errno) << std::endl;
			close(server.sockets.back());
			server.sockets.pop_back();
			failed_amount++;
			continue;
		}
	}

	if (failed_amount == server.listen.size())
		return (false);
	return (true);
}

// Setup the servers and call the event loop
void	launch_server(std::vector<Server>& servers)
{
	// Initialise all servers
	std::vector<Server>::iterator it = servers.begin();
	while (it != servers.end())
	{
		if (init_sockets(*it) == false)
		{
			std::cout << "An error occured while creating the server `" << it->server_name << "`" << std::endl;
			it = servers.erase(it);
		}
		else
			++it;
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
	event_loop(epfd);

	// Close when the server stop
	close(epfd);
}

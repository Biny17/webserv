#include "network.hpp"

// Creation and initialisation of the epoll instance to track new clients
int	init_epoll(int sockfd)
{
	// Create epoll instance
	int epfd = epoll_create(1);
	if (epfd == -1)
	{
		perror("epoll_create");
		return (-1);
	}

	// Add event to track new clients on the socket
	struct epoll_event event;
	event.events = EPOLLIN;	// Set for reading
	event.data.fd = sockfd;	// Attach server socket to epoll
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event) == -1)
	{
		perror("epoll_ctl: sockfd");
		close(epfd);
		return (-1);
	}

	return (epfd);
}

// Event loop of epoll
void	event_loop(int epfd, int sockfd)
{
	struct epoll_event events[MAX_EVENTS];

	while (true)
	{
		// Search tracked socket events
		int event_amount = epoll_wait(epfd, events, MAX_EVENTS, -1);	// Get events (blocking)
		if (event_amount == -1)
		{
			perror("epoll_wait");
			break;
		}

		// Iterate over events
		for (int i = 0; i < event_amount; i++)
		{
			int fd = events[i].data.fd;									// Get the fd of the socket of the event

			if (fd == sockfd)											// New client
				accept_new_client(epfd, sockfd);
			else														// Received form existing client
				read_client_data(epfd, fd);
		}
	}
}

// Setup the server and call the event loop
void	launch_server(void)
{
	// Create IPv4 TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		return ;
	}

	// Bind socket to port
	struct sockaddr_in addr;				// Address representation
	addr.sin_family = AF_INET;				// IPv4 addr
	addr.sin_port = htons(LISTENING_PORT);	// Set the port to listen on (htons convert to network endian)
	addr.sin_addr.s_addr = INADDR_ANY;		// Listen on all private network IPs
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		return ;
	}

	// Put socket in passive mode (ready to accept connections)
	if (listen(sockfd, SOMAXCONN) == -1)
	{
		perror("listen");
		return ;
	}

	// Create epoll instance and initialise it
	int epfd = init_epoll(sockfd);
	if (epfd == -1)
		return ;

	// Start the epoll event loop (wait for connections)
	event_loop(epfd, sockfd);

	// Close when the server stop
	close(epfd);
	close(sockfd);
}

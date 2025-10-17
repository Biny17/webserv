#include "network.hpp"

// Accept the client and set it as input
void	accept_new_client(int epfd, int sockfd)
{
	int	client_fd = accept(sockfd, NULL, NULL);
	if (client_fd == -1)
		return ;

	struct epoll_event cli_event;
	cli_event.events = EPOLLIN;
	cli_event.data.fd = client_fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &cli_event);
}

// Clear the client and remove it from epoll
void	disconnect_client(int epfd, int clifd)
{
	close(clifd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, clifd, NULL);
}

// Handle client request
void	read_client_data(int epfd, int clifd)
{
	char	buf[1024];
	int		bytes = recv(clifd, buf, sizeof(buf), 0);

	if (bytes <= 0) // Client exiting
	{
		disconnect_client(epfd, clifd);
		return ;
	}

	// Handle request
	send(clifd, buf, bytes, 0);
}
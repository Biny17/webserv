#include "webserv.hpp"

// Accept the client and set it as input
void	accept_new_client(int epfd, int sockfd, Server& server)
{
	// Create a socket for the client that is trying to connect to the server
	int	client_fd = accept(sockfd, NULL, NULL);
	if (client_fd == -1)
		return ;

	// Set the socket of the client as reading form that socket
	struct epoll_event cli_event;
	cli_event.events = EPOLLIN;
	cli_event.data.fd = client_fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &cli_event);
	server.addClient(client_fd);
}

// Handle client request
void	read_client_data(int epfd, int clifd, Server& server)
{
	Client& client = server.clients[clifd];

	if (client.isCGI == true)
	{
		listen_cgi(server, client);
		return ;
	}

	char	buf[REQUEST_BUFF_SIZE];
	int		bytes = recv(clifd, buf, sizeof(buf), 0);

	if (bytes == -1)
		perror("recv");

	if (bytes <= 0) // Client disconnecting
	{
		disconnect_client(epfd, clifd, server);
		return ;
	}

	buf[bytes] = 0;

	client.parse.FillReq(buf);
	client.parse.Print();
	client.parse.Reset();
	if (send_response(clifd, server) == false)
		set_epoll_event(epfd, clifd, EPOLLOUT);
}

bool	send_response(int clifd, Server& server)
{
	(void)clifd;
	(void)server;
	return (true);
}

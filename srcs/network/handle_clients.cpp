#include "webserv.hpp"
#include <fcntl.h>

// Accept the client and set it as input
void	accept_new_client(int sockfd, Server& server)
{
	// Create a socket for the client that is trying to connect to the server
	int	client_fd = accept(sockfd, NULL, NULL);
	if (client_fd == -1)
		return ;

	// Set the socket of the client as reading form that socket
	struct epoll_event cli_event;
	cli_event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	cli_event.data.fd = client_fd;
	if (epoll_ctl(server.epfd, EPOLL_CTL_ADD, client_fd, &cli_event) == -1)
	{
		close(client_fd);
		return;
	}
	server.addClient(client_fd);
	std::map<int, Client>::iterator	clientIt = server.clients.find(client_fd);
	clientIt->second.epollStatus = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
}

// Handle client request
void	read_client_data(Client& client, Server& server)
{
	char	buf[REQUEST_BUFF_SIZE + 1];
	ssize_t	bytes = recv(client.fd, buf, REQUEST_BUFF_SIZE, 0);

	if (bytes <= 0)
	{
		server.removeClient(client.fd);
		return ;
	}

	buf[bytes] = 0;
	size_t i = 0;
	if (client.parser.state == METHOD || client.parser.state == PATH
		|| client.parser.state == QUERY || client.parser.state == VERSION
		|| client.parser.state == HEAD_KEY || client.parser.state == HEAD_VAL)
	{
		i = client.parser.FillReq(buf);
	}
	if (client.parser.state == CHECK)
	{
		client.checkLocation();
	}
	if (client.parser.state == BODY)
	{
		(client.parser.*client.parser.f)(buf, i);
	}
	if (client.parser.state == HANDLE)
	{
		client.RequestHandler();
	}
	if (client.parser.state == RESPONSE || client.parser.state == ERROR) {
		client.response.Build();
		client.response.Send();
	}
	// if (client.request.path.find("cookie") != client.request.path.npos) // Temporary for cookies
	// 	client.switchCat();
}

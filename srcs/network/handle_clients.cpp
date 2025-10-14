#include "webserv.hpp"

// Accept the client and set it as input
void	accept_new_client(int epfd, int sockfd, std::map<int, int>& fd_map)
{
	int	client_fd = accept(sockfd, NULL, NULL);
	if (client_fd == -1)
		return ;

	struct epoll_event cli_event;
	cli_event.events = EPOLLIN;
	cli_event.data.fd = client_fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &cli_event);
	fd_map[client_fd] = client_fd;
}

// Clear the client and remove it from epoll
void	disconnect_client(int epfd, int clifd, std::map<int, int>& fd_map)
{
	close(clifd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, clifd, NULL);
	fd_map.erase(clifd);
}

// Handle client request
void	read_client_data(int epfd, int clifd, std::map<int, int>& fd_map)
{
	char	buf[REQUEST_BUFF_SIZE];
	int		bytes = recv(clifd, buf, sizeof(buf), 0);

	if (bytes <= 0) // Client exiting
	{
		disconnect_client(epfd, clifd, fd_map);
		return ;
	}

	// Handle request
	parse_request(buf, clifd);
}

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
	cli_event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;;
	cli_event.data.fd = client_fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &cli_event) == -1)
	{
		close(client_fd);
		return;
	}
	server.addClient(client_fd);
}

// Handle client request
void	read_client_data(int epfd, int clifd, Server& server)
{
	char	buf[REQUEST_BUFF_SIZE + 1];
	ssize_t	bytes = recv(clifd, buf, REQUEST_BUFF_SIZE, 0);

	Client&	client = server.clients[clifd];

	if (bytes == -1)
	{
		perror("recv");
		disconnect_client(epfd, clifd, server);
		return ;
	}

	if (bytes == 0)
	{
		if (client.out_buffer.empty())
			disconnect_client(epfd, clifd, server);
		return ;
	}

	buf[bytes] = 0;

	// std::cout << buf << std::endl;
	// Parse the request
	client.parser.FillReq(buf);

	if (client.parser.state != COMPLETE && client.parser.state != ERROR)
		return ;

	// Handle request
	client.parser.Print();
	std::cout << std::endl;
	client.RequestHandler();
	client.parser.Reset();
	// Fill response

	if (send_response(clifd, client.out_buffer) == false)
		set_epoll_event(epfd, clifd, EPOLLOUT);

	if (client.request.headers.find("Connection") != client.request.headers.end() && client.request.headers["Connection"] != "keep-alive")
		disconnect_client(epfd, clifd, server);
}


//checkpoint for every request (supposed to build the client.respond)
bool	handle_request(Server &server, Client &client, Request const &request, Response &response) {
	(void)client;
	int	checker;
	checker = check_allowed_methods(server, request.path, request.method);
	if (checker == 0) {
		if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
			response.error_code = 501;
		else
			response.error_code = 405;
		return (false);
	}
	else if (checker == -1) {
		response.error_code = 404;
		return (false);
	}

	//maybe more to verify, it depends of the request method
	if (request.method == "GET")
		;
	else if (request.method == "POST")
		;
	else if (request.method == "DELETE")
		;
	else {
		std::cout << "Unkown method" << std::endl;
		return (false);
	}
	return (true);
}

// Send the server's response to the client
bool	send_response(int clifd, std::string& out_buffer)
{
	ssize_t	bytes = send(clifd, out_buffer.c_str(), out_buffer.size(), 0);

	if (bytes > 0)
		out_buffer.erase(0, bytes);

	if (out_buffer.empty())
		return (true);

	return (false);
}

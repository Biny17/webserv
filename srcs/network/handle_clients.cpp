#include "webserv.hpp"
#include <fcntl.h>

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

	//request 
	std::cout << client.request.method << " " << client.request.path << " " << client.request.version << std::endl;
	
	// request headers;
	std::map<std::string, std::string>::iterator	it;
	std::map<std::string, std::string>::iterator	ite = client.request.headers.end();

	for (it = client.request.headers.begin(); it != ite; ++it)
	{
		std::cout << it->first << "----" << it->second << std::endl;
	}

	//checkpoint for every request (supposed to build the client.respond)
	if (handle_request(server, client, client.request, client.response) == false)
		;			// error

	// if finished
	if (send_response(clifd, client.out_buffer) == false)
		set_epoll_event(epfd, clifd, EPOLLOUT);

	// if finished
	if (send_response(clifd, client.out_buffer) == false)
		set_epoll_event(epfd, clifd, EPOLLOUT);

	// if Connection: close && (parse finished || parse failed)
	// disconnect_client(epfd, clifd, server);

}

std::string	find_index(std::vector<std::string> &index_page) {

	std::vector<std::string>::iterator	it;
	std::vector<std::string>::iterator	ite = index_page.end();

	for (it = index_page.begin(); it != ite; ++it)
	{
		if ((*it).find_last_of('.') != (*it).npos)
		{
			if ((*it).substr((*it).find_last_of('.'), (*it).size()) == ".html")
				return (*it);
		}
	}
	return("");
}

std::string	read_index(std::vector<std::string> &index_page, Server const &server, Location const &location) {
	std::string							index;
	std::string							buffer(1, '\0');
	std::string							result;
	int									fd;

	index = find_index(index_page);
	if (index == "")
		return ("");
	if (!location.root.empty())
		fd = open((location.root + location.path + index).c_str(), O_RDONLY);
	else
		fd = open((server.root + location.path + index).c_str(), O_RDONLY);
	if (fd == -1)
		return (perror("open"), "");
	while (read(fd, &buffer[0], 1))
		result += buffer;
	close (fd);
	return (result);
}

bool	build_get_response(Server &server, Client &client, Request const &request, Response &response) {
	(void)client;

	std::string status_line = request.version + " " + response.status_message + "\r\n";
	std::string	msg;
	std::cout << status_line << std::endl;
	if (!(*server.locations.begin()).index.empty())
	{
		msg = read_index((*server.locations.begin()).index, server, *server.locations.begin());
		if (msg == "")
			return (false);
		std::cout << msg << std::endl;
	}
	else
		std::cout << "autoindex" << std::endl;
	return (true);
}

//checkpoint for every request (supposed to build the client.respond)
bool	handle_request(Server &server, Client &client, Request const &request, Response &response) {
	(void)client;
	int	checker;
	checker = check_allowed_methods(server, request.path, request.method);
	if (checker == 0) {
		if (request.method != "GET" && request.method != "POST" && request.method != "DELETE") {
			response.code = 501;
			response.status_message = "501 Not Implemented";
		}
		else {
			response.code = 405;
			response.status_message = "405 Method Not Allowed";
		}
	}
	else if (checker == -1) {
		response.code = 404;		// not found
		response.status_message = "404 Not Found";
	}
	else
	{
		response.code = 200;		// status code OK, no error
		response.status_message = "200 OK";
	}

	//maybe more to verify, it depends of the request method
	if (request.method == "GET")
		build_get_response(server, client, request, response);
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

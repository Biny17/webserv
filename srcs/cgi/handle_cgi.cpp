#include "webserv.hpp"

// Read a CGI and add append it's string
void	read_cgi(char* buf, Client& client)
{
	client.out_buffer += buf;
}

// waitpid for the cgi's subprocess when finished
void	wait_cgi(Client& client)
{
	int		status;
	pid_t	pid = waitpid(client.CGIpid, &status, WNOHANG);

	if (pid == -1)
	{
		perror("waitpid");
		return ;
	}

	if (pid == 0)
		return ;

	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		std::cout << WEXITSTATUS(status) << std::endl;	// Handle the error here
}

void	build_cgi_response(std::string& result)
{
	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(result.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n" +
		result;
	result = response;
}

// Handle the CGI Request
void	listen_cgi(Server& server, Client& client)
{
	char	buf[REQUEST_BUFF_SIZE + 1];
	int		bytes = read(client.fd, buf, REQUEST_BUFF_SIZE);

	if (bytes < 0)
		return ;

	if (bytes == 0)
	{
		wait_cgi(client);

		std::map<int, Client>::iterator referringClient = server.clients.find(client.referringFD);
		if (referringClient == server.clients.end())
		{
			disconnect_client(server.epfd, client.fd, server);
			return;
		}

		referringClient->second.out_buffer = client.out_buffer;
		build_cgi_response(referringClient->second.out_buffer);

		if (send_response(client.referringFD, referringClient->second.out_buffer) == false) // Add http header here
			set_epoll_event(server.epfd, client.referringFD, EPOLLOUT);

		disconnect_client(server.epfd, client.fd, server);
		return ;
	}

	buf[bytes] = 0;
	read_cgi(buf, client);
	return ;
}

void	add_cgi(Server& server, Client& client, std::string& filename)
{
	int cgiFD = launch_cgi(filename, client, __environ);

	if (cgiFD < 0)
		return ;

	server.addClient(cgiFD);
	server.clients[cgiFD].setCGI(client.fd, server);
}

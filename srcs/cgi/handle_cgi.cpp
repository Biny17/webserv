#include "webserv.hpp"

// Read a CGI and add append it's string
void	read_cgi(char* buf, Client& client)
{
	client.cgi_body += buf;
}

// Handle the CGI Request
void	listen_cgi(Server& server, Client& client)
{
	char	buf[REQUEST_BUFF_SIZE + 1];
	int		bytes = read(client.fd, buf, sizeof(buf));

	if (bytes <= 0)
	{
		int status;
		if (waitpid(client.CGIpid, &status, 0) == -1)
		{
			perror("waitpid");
			server.removeClient(client.fd);
			return ;
		}
		if (bytes < 0)
		{
			if (WIFEXITED(status))
				std::cout << WEXITSTATUS(status) << std::endl;
		}
		else
			std::cout << "CGI response: " << client.cgi_body << std::endl; // Make the response here
		server.removeClient(client.fd);
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

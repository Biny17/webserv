#include "webserv.hpp"
#include <sstream>

// Read a CGI and add append it's string
void	read_cgi(char* buf, Client& cgi)
{
	cgi.response.body += buf;
}

// waitpid for the cgi's subprocess when finished
void	wait_cgi(Client& cgi)
{
	int		status;
	pid_t	pid = waitpid(cgi.CGIpid, &status, WNOHANG);

	if (pid == -1)
	{
		perror("waitpid");
		return ;
	}

	if (pid == 0)
		return ;

	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		cgi.response.code = 502;
		cgi.response.body = "Cannot execute the CGI";
	}
}

void	send_cgi(Server& server, Client& cgi)
{
	wait_cgi(cgi);

	std::map<int, Client>::iterator	clientIt = server.clients.find(cgi.referringFD);
	if (clientIt == server.clients.end())
	{
		server.removeClient(cgi.fd);
		return;
	}

	Client&	client = clientIt->second;
	client.response.body = cgi.response.body;
	client.response.code = cgi.response.code;
	if (client.response.code == 200)
		client.response.BuildCGI();
	else
		client.response.Build();
	client.response.Send();
}

// Handle the CGI Request
void	listen_cgi(Server& server, Client& cgi)
{
	char	buf[REQUEST_BUFF_SIZE + 1];
	int		bytes = read(cgi.fd, buf, REQUEST_BUFF_SIZE);

	if (bytes < 0)
		return ;

	if (bytes == 0)
	{
		send_cgi(server, cgi);
		server.removeClient(cgi.fd);
		return ;
	}

	buf[bytes] = 0;
	read_cgi(buf, cgi);
	return ;
}

void	kill_cgi(Client& cgi)
{
	kill(cgi.CGIpid, SIGKILL);
	wait_cgi(cgi);

	std::map<int, Client>::iterator	clientIt = cgi.server.clients.find(cgi.referringFD);
	if (clientIt == cgi.server.clients.end())
		return;

	Client&	client = clientIt->second;
	client.response.body = "CGI has been timeout";
	client.response.code = 504;
	client.response.Build();
	client.response.Send();
}

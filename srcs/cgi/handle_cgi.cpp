#include "webserv.hpp"

// Read a CGI and add append it's string
void	read_cgi(char* buf, Client& cgi)
{
	cgi.response.outBuffer += buf;
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
		std::cout << WEXITSTATUS(status) << std::endl;	// Handle the error here
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
		wait_cgi(cgi);

		std::map<int, Client>::iterator	clientIt = server.clients.find(cgi.referringFD);
		if (clientIt == server.clients.end())
		{
			server.removeClient(cgi.fd);
			return;
		}

		Client&	client = clientIt->second;
		client.response.outBuffer = client.response.outBuffer;
		client.response.Send();

		server.removeClient(client.fd);
		return ;
	}

	buf[bytes] = 0;
	read_cgi(buf, cgi);
	return ;
}

void	add_cgi(Server& server, Client& client, std::string& filename)
{
	int cgiFD = launch_cgi(filename, client, __environ);

	if (cgiFD < 0)
		return ;

	server.addClient(cgiFD);
	std::map<int, Client>::iterator clit = server.clients.find(cgiFD);
	clit->second.setCGI(client.fd);
}

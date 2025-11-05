#include "webserv.hpp"

std::string	find_path(std::string const &cmd, char **env) {
	int			i = 0;
	bool		flag = false;
	std::string	path;
	while (env[i])
	{
		std::string	line = env[i];
		if (line.substr(0, 4) == "PATH") {
			path = env[i];
			break;
		}
		i++;
	}
	if (path.empty())
		return ("");
	std::string	test;
	for(std::string::iterator it = path.begin() + 5; it != path.end(); ++it) {
		if (*it == ':') {
			test += ("/" + cmd);
			if (!access(test.c_str(), F_OK)) {
				flag = true;
				break;
			}
			test.clear();
		}
		else
			test.push_back(*it);
	}
	if (flag == false)
		return ("");
	return (test);
}

void	close_fds(void)
{
	std::vector<Server>::iterator serv;
	std::vector<Server>::iterator serve = servers.end();
	for (serv = servers.begin(); serv != serve; ++serv)
	{
		if (serv->epfd >= 0)
		{
			close(serv->epfd);
			serv->epfd = -1;
		}
		serv->clients.clear();
		std::vector<int>::iterator sock;
		std::vector<int>::iterator socke = serv->sockets.end();
		for (sock = serv->sockets.begin(); sock != socke; ++sock)
		{
			if (*sock >= 0)
				close(*sock);
		}
	}
}

void	exec_child(std::vector<std::string> const &cgi, Client& client, int	fdout, int pipefd[2])
{
	char	*tab[] = {(char *)cgi[0].c_str(), (char *)cgi[1].c_str(), NULL};

	std::vector<char*>	env;
	for (char **current = environ; *current; ++current) {
		env.push_back(*current);
	}
	std::string query = "QUERY_STRING=" + client.request.query;
	env.push_back(const_cast<char *>(query.c_str()));
	if (client.request.method == "POST")
	{
		std::string length = "CONTENT-LENGTH=" + client.request.headers["Content-Length"];
		env.push_back(const_cast<char *>(length.c_str()));
		// dup2(client.request.body, STDIN_FILENO);
	}
	env.push_back(NULL);
	close_fds();
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	close(fdout);
	std::string test = find_path(cgi[0], __environ);
	execve(test.c_str(), tab, env.data());
	std::string errmessage = "cgi execve failure - ";
	errmessage += strerror(errno);
	throw std::runtime_error(errmessage.c_str());
}

int	exec_cgi(std::vector<std::string> const &cgi, Server& server, Client& client)
{
	int		pipefd[2];
	int		fdout;
	pid_t	pid;

	pipe(pipefd);
	fdout = dup(pipefd[0]);
	server.addClient(fdout);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
		exec_child(cgi, client, fdout, pipefd);
	std::map<int, Client>::iterator cliIt = server.clients.find(fdout);
	cliIt->second.setCGI(client.fd);
	cliIt->second.CGIpid = pid;
	close(pipefd[1]);
	close(pipefd[0]);
	return (fdout);
}

int	launch_cgi(std::string &filename, Server& server, Client& client) {

	std::vector<std::string>	cgi;
	if (access(filename.c_str(), F_OK))
		return (-1);
	if (filename.find_last_of('.') == filename.npos)
		return (-1);
	if (filename.substr(filename.find_last_of('.'), filename.size()) == ".py") {
		cgi.push_back("python");
		cgi.push_back(filename);
	}
	else if (filename.substr(filename.find_last_of('.'), filename.size()) == ".sh") {
		cgi.push_back("sh");
		cgi.push_back(filename);
	}
	else
		return (-1);
	return(exec_cgi(cgi, server, client));
}

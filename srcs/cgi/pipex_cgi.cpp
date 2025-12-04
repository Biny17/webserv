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

void	exec_child(std::vector<std::string> const &cgi, Client& client, int	fdout, int pipe_in[2], int pipe_out[2])
{
	char	*tab[] = {(char *)cgi[0].c_str(), (char *)cgi[1].c_str(), NULL};

	std::vector<std::string> env;
	for (char **current = environ; *current; ++current) {
		env.push_back(*current);
	}
	env.push_back("SERVER_NAME=" + client.server.server_name);
	env.push_back("REQUEST_METHOD=" + client.request.method);
	env.push_back("SCRIPT_NAME=" + client.request.path);
	if (client.request.method == "GET")
		env.push_back("QUERY_STRING=" + client.request.query);
	if (client.request.method == "POST")
	{
		env.push_back("CONTENT_LENGTH=" + client.request.headers["Content-Length"]);
		env.push_back("CONTENT_TYPE=" + client.request.headers["Content-Type"]);

		dup2(pipe_in[0], STDIN_FILENO);

	}

	std::vector<char*> envp;
	envp.reserve(env.size() + 1);
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it)
		envp.push_back(const_cast<char*>(it->c_str()));
	envp.push_back(NULL);

	close_fds();
	close(pipe_in[1]);
	close(pipe_in[0]);
	close(pipe_out[0]);
	dup2(pipe_out[1], STDOUT_FILENO);
	close(pipe_out[1]);
	close(fdout);
	std::string test = find_path(cgi[0], __environ);
	execve(test.c_str(), tab, envp.data());
	std::string errmessage = "cgi execve failure - ";
	errmessage += strerror(errno);
	throw std::runtime_error(errmessage.c_str());
}

int	exec_cgi(std::vector<std::string> const &cgi, Server& server, Client& client)
{
	int		pipe_out[2];
	int		pipe_in[2];
	int		fdout;
	pid_t	pid;

	if (pipe(pipe_out) == -1)
		return (-1);
	if (pipe(pipe_in) == -1) {
		close(pipe_out[0]); close(pipe_out[1]);
		return (-1);
	}
	fdout = dup(pipe_out[0]);
	server.addClient(fdout);
	pid = fork();
	if (pid == -1) {
		close(pipe_out[0]); close(pipe_out[1]);
		close(pipe_in[0]); close(pipe_in[1]);
		close(fdout);
		return (-1);
	}
	if (pid == 0)
		exec_child(cgi, client, fdout, pipe_in, pipe_out);

	if (client.request.method == "POST")
	{
		const char *buf = client.request.body.c_str();
		size_t to_write = client.request.body.size();
		long written = 0;
		while (to_write > 0) {
			long n = write(pipe_in[1], buf + written, to_write);
			if (n <= 0)
				break;
			written += n;
			to_write -= n;
		}
	}

	close(pipe_in[1]);
	close(pipe_in[0]);
	close(pipe_out[1]);
	close(pipe_out[0]);

	std::map<int, Client>::iterator cliIt = server.clients.find(fdout);
	cliIt->second.setCGI(client.fd);
	cliIt->second.CGIpid = pid;
	return (fdout);
}

int	launch_cgi(std::string &filename, Server& server, Client& client) {

	std::vector<std::string>	cgi;
	if (access(filename.c_str(), F_OK))
		return (404);
	if (filename.find_last_of('.') == filename.npos)
		return (501);
	if (filename.substr(filename.find_last_of('.'), filename.size()) == ".py") {
		cgi.push_back("python");
		cgi.push_back(filename);
	}
	else if (filename.substr(filename.find_last_of('.'), filename.size()) == ".sh") {
		cgi.push_back("sh");
		cgi.push_back(filename);
	}
	else
		return (501);
	return(exec_cgi(cgi, server, client)); // quoi faire avec ce mysterieux fd ?
}

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

int	exec_cgi(std::vector<std::string> const &cgi, Client& client, char **env) {

	char	*tab[] = {(char *)cgi[0].c_str(), (char *)cgi[1].c_str(), NULL};
	int		pipefd[2];
	int		fdout;
	pid_t	pid;

	pipe(pipefd);
	fdout = dup(pipefd[0]);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0) {
		close_fds();
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		close(fdout);
		std::string test = find_path(cgi[0], env);
		execve(test.c_str(), tab, env);
		std::string errmessage = "cgi execve failure - ";
		errmessage += strerror(errno);
		throw std::runtime_error(errmessage.c_str());
	}
	client.CGIpid = pid;
	close(pipefd[1]);
	close(pipefd[0]);
	return (fdout);
}

int	launch_cgi(std::string &filename, Client& client, char **env) {

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
	return(exec_cgi(cgi, client, env));
}

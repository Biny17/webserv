#include "webserv.hpp"
#include <fcntl.h>
#include <cstdio>

std::string	rebuild_path(Server const &server, Location const &location, Request const &request) {

	std::string	build;

	if (request.path == location.path)
	{
		if (!location.root.empty())
			build = location.root + location.path;
		else
			build = server.root + location.path;
		return (build);
	}
	if (!location.root.empty())
		build = location.root + location.path + request.path;
	else
		build = server.root + location.path + request.path;
	return (build);
}

void	build_delete_response(Server &server, Request &request, Response &response) {
	(void)response;

	std::cout << "here" << std::endl;
	std::string	file;

	file = rebuild_path(server, server.locations[request.loc_index], request);

	int fd = open(file.c_str(), O_RDONLY);
	if (fd != -1)
	{
		close(fd);
		fd = open(file.c_str(), O_DIRECTORY);
		if (fd != -1)
		{
			std::cout << "C'EST UN DIRECTORY" << std::endl;
			return ;
		}
		close (fd);
	}
	else
	{
		std::cout << "marche pas" << std::endl;
		return ;
	}
	// std::remove(file.c_str());
	std::cout << file << std::endl;
	std::cout << "OMG" << std::endl;
	return ;
}
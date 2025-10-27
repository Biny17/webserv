#include "webserv.hpp"
#include <fcntl.h>

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

	close(fd);
	return (result);
}

void	build_get_response(Server &server, Client &client, Request const &request, Response &response) {
	(void)client;
	(void)request;

	if (!(*server.locations.begin()).index.empty())
	{
		response.body = read_index((*server.locations.begin()).index, server, *server.locations.begin());	//response body
		if (response.body == "")
			response.body = autoindex(request.path_from_root);
	}
	else
		response.body = autoindex(request.path_from_root);
	return ;
}
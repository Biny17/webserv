#include "webserv.hpp"
#include <fcntl.h>

std::string	find_index(std::vector<std::string> const &index_page) {

	std::vector<std::string>::const_iterator	it;
	std::vector<std::string>::const_iterator	ite = index_page.end();
	std::string									result;

	for (it = index_page.begin(); it != ite; ++it)
	{
		if ((*it).find_last_of('.') != (*it).npos)
		{
			if ((*it).substr((*it).find_last_of('.'), (*it).size()) == ".html")
				result = *it;
		}
	}
	if (result.empty())
		return (*index_page.begin());
	return(result);
}

std::string	read_index(std::string const &path, Server const &server, Location const &location) {

	std::string	index;
	std::string	buffer(1, '\0');
	std::string	result;
	int			fd;


	if (path == location.path)
		index = find_index(location.index);
	else
		index = path;
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

//creation du response.content_type (NEED LE CHARSET ENCORE PAS FINI)
std::string	file_extension(std::string const &path, Location const &location) {

	std::string	ext;
	if (path == location.path)
		ext = find_index(location.index);
	else
		ext = path;

	if (ext.find_last_of('.') != ext.npos)
		ext = ext.substr(ext.find_last_of('.'), ext.size());
	if (location.extension.find(ext) != location.extension.end())
		ext = location.extension.find(ext)->second;
	else
		return ("");
	return (ext + "; charset=utf-8");
}

void	get_static_file(Server &server, Client &client, Request const &request, Response &response) {
	(void)client;

	if (!(*server.locations.begin()).index.empty())
	{
		// response.body = read_index(request.path, server, find_location(request.path, server.locations)/**server.locations.begin()*/);	//response body
		response.body = read_index(request.path, server, server.locations[request.loc_index]/**server.locations.begin()*/);	//response body
		if (response.body == "")
			response.body = autoindex(request.local_path);
	}
	else
		response.body = autoindex(request.local_path);
	return ;
}
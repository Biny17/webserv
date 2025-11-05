#include "webserv.hpp"
#include<fcntl.h>

std::string	rebuild_path(Server const &server, Location const &location, std::string const &path) {

	std::string	build;

	if (path == location.path)
	{
		std::cout << "feur" << std::endl;
		if (!location.root.empty())
			build = location.root + location.path;
		else
			build = server.root + location.path;
		return (build);
	}
	else if (path.find(location.path) != path.npos)
	{
		if (!location.root.empty())
			build = location.root + path;
		else
			build = server.root + path;
	}
	else
	{
		if (!location.root.empty())
			build = location.root + location.path + path;
		else
			build = server.root + location.path + path;
	}
	return (build);
}

//check if the content given in path is a directory or a file,
//ret value :
//1 if file, 2 if directory, 0 if undefined
int		content_type(std::string const &path, Server const &server, Location const &location) {
	std::string	content;
	
	content = rebuild_path(server, location, path);

	std::cout << content << std::endl;
	if (access(content.c_str(), F_OK) == -1)		//return 0 if undefined
		return (0);
	
	int fd = open(content.c_str(), O_DIRECTORY);	//return 2 if directory
	if (fd != -1)
		return (close(fd), 2);

	fd = open(content.c_str(), O_RDONLY);			//return 1 if file
	if (fd != -1)
		return (close(fd), 1);
	return (0);
}
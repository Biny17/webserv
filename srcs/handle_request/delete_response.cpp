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

void	build_delete_response(Server const &server, Request const &request, Response &response) {

	std::string	file;

	file = rebuild_path(server, server.locations[request.loc_index], request);			//rebuild path to find the file

	if (access(file.c_str(), F_OK) == -1)												//check if the file exist
	{
		response.code = 400;	//peut etre 404 je sais pas vraiment
		return ;
	}

	int fd = open(file.c_str(), O_RDONLY);
	if (fd != -1)
	{
		close(fd);
		fd = open(file.c_str(), O_DIRECTORY);	//verifie si file est un directory si fd != -1 alors file est un directory
		if (fd != -1)
		{
			response.code = 400;				//si directory -> 400 Bad request
			close (fd);
			return ;
		}
		std::remove(file.c_str());				//remove le fichier
		response.code = 204;					//plusieurs codes possible : 200, 202, 204. celui de base est 204 comme ca pas de body
	}
	else
		response.code = 403;					//si le fichier existe mais qu'il n'est pas accessible -> 403 Forbidden
	return ;
}

//mauvais parametre : 400
//pas autorise : 403
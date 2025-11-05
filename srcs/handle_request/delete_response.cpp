#include "webserv.hpp"
#include <fcntl.h>
#include <cstdio>

void	build_delete_response(Server const &server, Request const &request, Response &response) {

	std::string	file;

	file = rebuild_path(server, server.locations[request.loc_index], request.path);			//rebuild path to find the file

	if (access(file.c_str(), F_OK) == -1)												//check if the file exist
	{
		response.code = 404;
		return ;
	}

	int content = content_type(request.path, server, server.locations[request.loc_index]);
	if (content == 2)
		response.code = 400;
	else if (content == 0)
		response.code = 403;
	else
	{
		std::remove(file.c_str());
		response.code = 204;
	}

	return ;
}

	// int fd = open(file.c_str(), O_RDONLY);
	// if (fd != -1)
	// {
	// 	close(fd);
	// 	fd = open(file.c_str(), O_DIRECTORY);	//verifie si file est un directory si fd != -1 alors file est un directory
	// 	if (fd != -1)
	// 	{
	// 		response.code = 400;				//si directory -> 400 Bad request
	// 		close (fd);
	// 		return ;
	// 	}
	// 	std::remove(file.c_str());				//remove le fichier
	// 	response.code = 204;					//plusieurs codes possible : 200, 202, 204. celui de base est 204 comme ca pas de body
	// }
	// else
	// 	response.code = 403;					//si le fichier existe mais qu'il n'est pas accessible -> 403 Forbidden

//mauvais parametre : 400
//pas autorise : 403
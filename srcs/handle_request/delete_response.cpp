#include "webserv.hpp"
#include <fcntl.h>
#include <cstdio>

void	build_delete_response(Server const &server, Request const &request, Response &response) {

	(void)server;
	if (access(request.path_from_root.c_str(), F_OK) == -1)												//check if the file exist
	{
		response.code = 404;
		return ;
	}

	int content = content_type(request.path_from_root);
	if (content == 2)
		response.code = 400;
	else if (content == 0)
		response.code = 403;
	else
	{
		std::remove(request.path_from_root.c_str());
		response.code = 204;
	}

	return ;
}
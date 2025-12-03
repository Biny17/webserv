#include "webserv.hpp"
#include <cstdio>

void	build_delete_response(Server const &server, Request const &request, Response &response) {

	(void)server;
	if (request.local_path.find("..") != request.local_path.npos)
	{
		response.code = 400;
		return ;
	}
	if (access(request.local_path.c_str(), F_OK) == -1)												//check if the file exist
	{
		response.code = 404;
		return ;
	}

	int content = target_type(request.local_path);
	if (content == 2)
		response.code = 400;
	else if (content == 0)
		response.code = 403;
	else
	{
		std::remove(request.local_path.c_str());
		response.code = 204;
	}

	return ;
}
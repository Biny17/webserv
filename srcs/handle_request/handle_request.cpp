#include "webserv.hpp"
#include <fcntl.h>

//checkpoint for every request (supposed to build the client.respond)
void	handle_request(Server &server, Client &client, Request &request, Response &response) {
	(void)client;

	int checker = check_allowed_methods(server, request.path, request.method, request);
	if (checker == 0) {
		if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
			response.code = 501;
		else
			response.code = 405;
		return ;
	}
	//maybe more to verify, it depends of the request method
	if (request.method == "GET") {
		if (checker == -1) {
			response.code = 404;
			return ;
		}
		build_get_response(server, request, response);	//build get response
	}
	else if (request.method == "POST")
		;
	else if (request.method == "DELETE")
		build_delete_response(server, request, response);	//build delete response
	else {
		response.code = 501;
		std::cout << "Unkown method" << std::endl;
		return ;
	}
	return ;
}

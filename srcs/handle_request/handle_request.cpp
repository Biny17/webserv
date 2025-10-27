#include "webserv.hpp"
#include <fcntl.h>

//checkpoint for every request (supposed to build the client.respond)
bool	handle_request(Server &server, Client &client, Request const &request, Response &response) {
	(void)client;
	int	checker;
	checker = check_allowed_methods(server, request.path, request.method);
	if (checker == 0) {
		if (request.method != "GET" && request.method != "POST" && request.method != "DELETE") {
			response.error_code = 501;
			response.status_message = "501 Not Implemented";
		}
		else {
			response.error_code = 405;
			response.status_message = "405 Method Not Allowed";
		}
	}
	else if (checker == -1) {
		response.error_code = 404;		// not found
		response.status_message = "404 Not Found";
	}
	else
	{
		response.error_code = 200;		// status code OK, no error
		response.status_message = "200 OK";
	}

	//maybe more to verify, it depends of the request method
	if (request.method == "GET")
		build_get_response(server, client, request, response);
	else if (request.method == "POST")
		;
	else if (request.method == "DELETE")
		;
	else {
		std::cout << "Unkown method" << std::endl;
		return (false);
	}
	return (true);
}
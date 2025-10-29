#include "webserv.hpp"
#include <fcntl.h>

//checkpoint for every request (supposed to build the client.respond)
void	handle_request(Server &server, Client &client, Request &request, Response &response) {
	(void)client;
	int	checker;
	std::cout << "here" << std::endl;

	checker = check_allowed_methods(server, request.path, request.method, request);
	std::cout << checker << std::endl;
	if (checker == 0) {
		if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
			response.code = 501;
		else
			response.code = 405;
		return ;
	}
	else if (checker == -1) {
		response.code = 404;		// not found
		return ;
	}
	else
		response.code = 200;		// status code OK, no error
	//maybe more to verify, it depends of the request method
	if (request.method == "GET")
		build_get_response(server, client, request, response);
	else if (request.method == "POST")
		;
	else if (request.method == "DELETE")	// 204 si le delete a eu lieu correctement et que rien n'a besoin d'etre envoye au client
											// 200 si le delte est ok et qu'on renvoie une information lie au delete au client
											// 202 si le delete est accepte mais pas encore delete
		build_delete_response(server, request, response);
	else {
		std::cout << "Unkown method" << std::endl;
		return ;
	}
	return ;
}

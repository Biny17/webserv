#include "webserv.hpp"



//check if method req_method is allowed in Location location
static bool	check_method(Location const &location, std::string const &req_method) {
	std::vector<std::string>::const_iterator	it;
	std::vector<std::string>::const_iterator	ite = location.methods.end();

	std::cout << location << std::endl;
	for (it = location.methods.begin(); it != ite; ++it)
	{
		if ((*it) == req_method)
			return (true);
	}
	return (false);
}

static bool	find_location(Server const &server, std::string const &req_path, std::string const &req_method) {
	std::vector<Location>::const_iterator	it;
	std::vector<Location>::const_iterator	ite = server.locations.end();
	for (it = server.locations.begin(); it != ite; ++it)
	{
		if (req_path.find((*it).path) != req_path.npos) {
			if (check_method((*it), req_method) == true)
				return (true);
			return (false);
		}
	}
	return (false);
}

//check if the request method "req_method" on Server "server" is allowed in path "req_path",
//it check every location of server and test if path reconstruction match,
//if match, check allowed method in the designed location
//ret_value :
//1 if method and path match, 0 if path but not method, -1 if path dont match
int	check_allowed_methods(Server const &server, std::string const &req_path, std::string const &req_method) {
	if (!access(req_path.c_str(), F_OK)) {
		std::cout << "absolute" << std::endl;
		if (find_location(server, req_path, req_method) == true)
			return (1);
		return (0);
	}
	if (!access((server.root + req_path).c_str(), F_OK)) {
		std::cout << "root + absolute" << std::endl;
		if (find_location(server, req_path, req_method) == true)
			return (1);
		return (0);
	}
	std::vector<Location>::const_iterator	it;
	std::vector<Location>::const_iterator	ite = server.locations.end();
	for (it = server.locations.begin(); it != ite; ++it)
	{
		std::string const path = server.root + (*it).root + (*it).path + req_path;
		std::cout << path << std::endl;
		if (!access(path.c_str(), F_OK)) {
			std::cout << "reconstruct " << path << std::endl;
			if (check_method((*it), req_method) == true)
				return (1);
			return (0);
		}
	}
	return (-1);
}

// bool	send_response(int clifd, Server& server)
// {

// 	return (true);
// }
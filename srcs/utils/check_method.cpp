#include "webserv.hpp"

// check if method req_method is allowed in Location location
static bool	check_method(Location const &location, std::string const &req_method) {
	std::vector<std::string>::const_iterator	it;
	std::vector<std::string>::const_iterator	ite = location.methods.end();

	for (it = location.methods.begin(); it != ite; ++it)
	{
		if ((*it) == req_method)
			return (true);
	}
	return (false);
}

//check if the request method "req_method" on Server "server" is allowed in path "req_path",
//it check every location of server and test if path reconstruction match,
//if match, check allowed method in the designed location
//ret_value :
//1 if method and path match, 0 if path but not method, -1 if path dont match
int	check_allowed_methods(Server const &server, std::string const &req_path, std::string const &req_method, Request &request) {
	std::vector<Location>::const_iterator	it;
	std::vector<Location>::const_iterator	ite = server.locations.end();
	std::vector<Location>::const_iterator	itbase;

	for(it = server.locations.begin(); it != ite; ++it)
	{
		std::string	path;
		if ((*it).path != "/" && req_path.find((*it).path) != req_path.npos)
		{
			if ((*it).root.empty())
				path = server.root + req_path;
			else
				path = (*it).root + req_path;
			std::cout << (*it) << std::endl;
			if (!access(path.c_str(), F_OK))
			{
				request.path_from_root = path;
				return(check_method(*it, req_method));
			}
			return (-1);
		}
		if ((*it).path == "/")
			itbase = it;
	}
	if (req_path.find((*itbase).path) != req_path.npos)
	{
		std::string	path;
		if ((*itbase).root.empty())
			path = server.root + req_path;
		else
			path = (*itbase).root + req_path;
		std::cout << *itbase << std::endl;
		if (!access(path.c_str(), F_OK))
		{
			request.path_from_root = path;
			return(check_method(*itbase, req_method));
		}
		return (-1);
	}
	return (-1);
}
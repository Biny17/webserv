#include "webserv.hpp"
#include <filesystem>

std::string	find_index(std::vector<std::string> const &index_page) {

	std::vector<std::string>::const_iterator	it;
	std::vector<std::string>::const_iterator	ite = index_page.end();
	std::string									result;

	for (it = index_page.begin(); it != ite; ++it)
	{
		if ((*it).find_last_of('.') != (*it).npos)
		{
			if ((*it).substr((*it).find_last_of('.'), (*it).size()) == ".html")
				result = *it;
		}
	}
	if (result.empty())
		return ("");
	return(result);
}

bool	fetch_file(std::string const &path, Location const &location, std::string& result)
{
	std::ifstream myfile;
	myfile.open(path);
	if (!myfile.is_open())
		return false;
	myfile.seekg(0, std::ios::end);
    result.reserve(myfile.tellg());
    myfile.seekg(0, std::ios::beg);

	result.assign((std::istreambuf_iterator<char>(myfile)),
				   std::istreambuf_iterator<char>());
	return true;
}

void	get_static_file(Server &server, Request const &request, Response &response)
{
	int content = target_type(request.local_path);
	std::string req_path = request.local_path;
	Location& location = server.locations[request.loc_index];

	if (content == 1) // 1 is file
	{
		if (!fetch_file(req_path, location, response.body))
			response.code = 500;
		else {
			response.content_type = Mime::GetType(get_extension(request.local_path));
			response.code = 200;
		}
	}
	else if (content == 2 && !location.index.empty())
	{
		if (!fetch_file(req_path, location, response.body))
			response.code = 500;
		else {
			response.content_type = "text/html";
			response.code = 200;
		}
	}
	else if (server.locations[request.loc_index].autoindex == 1)
		response.body = autoindex(request.local_path, request);
	else
		response.code = 404;
	return ;
}
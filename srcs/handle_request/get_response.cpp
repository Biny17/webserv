#include "webserv.hpp"
#include <filesystem>
#include <sys/stat.h>

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

bool	fetch_file(std::string const &path, std::string& result)
{
	std::ifstream myfile;
	struct stat stat_v;

	if (stat(path.c_str(), &stat_v) == -1 || !S_ISREG(stat_v.st_mode))
		return false;
	myfile.open(path.c_str());
	if (!myfile.is_open() || !myfile.good())
		return false;
    result.reserve(static_cast<size_t>(stat_v.st_size));
	result.assign((std::istreambuf_iterator<char>(myfile)),
				   std::istreambuf_iterator<char>());
	return true;
}

void	get_static_file(Request const &request, Response &response)
{
	Target::Type content = target_type(request.local_path);
	std::string req_path = request.local_path;
	Location& location = *request.location;

	if (content == Target::FILE)
	{
		if (!fetch_file(req_path, response.body))
			response.code = 500;
		else {
			response.content_type = Mime::GetType(get_extension(request.local_path));
			response.code = 200;
		}
	}
	else if (content == Target::DIR && !location.index.empty())
	{
		req_path += find_index(location.index);
		if (!fetch_file(req_path, response.body))
			response.code = 404;
		else {
			response.content_type = "text/html";
			response.code = 200;
		}
	}
	else if (content == Target::DIR && (*request.location).autoindex == true)
		response.body = autoindex(request.local_path, request);
	else if (content == Target::NOTFOUND)
		response.code = 404;
	else
		response.code = 500;
	return ;
}
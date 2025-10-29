#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <map>
#include <string>
#include "Location.hpp"

class Location;

struct Request {
	std::string method;
	std::string path;
	std::string query;
	std::string version;
	std::map<std::string, std::string> headers;
	int content_len;

	std::string	local_path;
	int			loc_index;
	std::string body;
};

#endif // REQUEST_HPP

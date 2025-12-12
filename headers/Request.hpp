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
	unsigned int content_len;

	std::string	local_path;
	std::string body;
	Location*	location;
};

#endif // REQUEST_HPP

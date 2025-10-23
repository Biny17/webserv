#pragma once
#include <map>
#include <string>

struct Request {
	std::string method;
	std::string path;
	std::string query;
	std::string version;
	std::map<std::string, std::string> headers;
	int content_len;
};

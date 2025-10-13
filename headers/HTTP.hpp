#pragma once
#include <map>
#include <string>

enum Method {
    NOT_HANDLED,
    GET,
    POST,
    DELETE
};

struct Http_req {
    Method method;
    std::string target;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};
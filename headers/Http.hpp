#pragma once
#include <map>
#include <string>

struct HttpReq {
    std::string method;
    std::string path;
    std::string query;
    std::string version;
    std::map<std::string, std::string> headers;
    size_t content_len;
    std::string body;
};

struct HttpError {
    int error_code;
    std::string msg;
};

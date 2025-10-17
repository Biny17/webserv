#pragma once
#include <map>
#include <string>

enum p_state {
    INIT,
    METHOD,
    PATH,
    VERSION,
    HEADERS,
    BODY,
    CHUNK_SIZE,
    CHUNK_DATA,
    CHUNK_DATA_CRLF,
    COMPLETE,
    ERROR_STATE
};

struct HttpReq {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    size_t content_len;
    std::string body;
};

struct HttpError {
    int error_code;
    std::string msg;
};

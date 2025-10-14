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
    p_state state;
    bool in_token;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    size_t content_len;
    std::string body;
};

void FillReq(HttpReq& req, const std::string& buf);
void Method(HttpReq&req, const std::string& buf);
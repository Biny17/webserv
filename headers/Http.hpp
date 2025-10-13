#pragma once
#include <map>
#include <string>

enum Method {
    ERROR,
    GET,
    POST,
    DELETE
};

enum p_state {
    INIT,
    REQUEST_LINE,
    HEADERS,
    HEADER_NAME,
    HEADER_VALUE,
    HEADERS_END,
    BODY,
    CHUNK_SIZE,
    CHUNK_DATA,
    CHUNK_DATA_CRLF,
    COMPLETE,
    ERROR_STATE
};

struct HttpReq {
    Method method;
    p_state state;
    std::string target;
    std::string version;
    std::map<std::string, std::string> headers;
    size_t content_len;
    std::string body;
};

void FillReq(HttpReq& req, const std::string &buf);
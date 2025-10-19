#pragma once
#include "Http.hpp"

enum p_state {
    INIT,
    METHOD,
    PATH,
    QUERY,
    VERSION,
    HEADERS,
    BODY,
    CHUNK_SIZE,
    CHUNK_DATA,
    CHUNK_DATA_CRLF,
    COMPLETE,
    ERROR
};

struct ParseResult {
    bool ok;
    HttpReq req;
    HttpError err;
    p_state state;
    int pct;

    ParseResult();
    void Error(std::string msg, int error_code);
    void Method(const std::string& buff, size_t& i);
    void Path(const std::string& buff, size_t& i);
    void Query(const std::string &buff, size_t& i);
    void Version(const std::string &buff, size_t& i);
    void FillReq(const std::string& buff);
};
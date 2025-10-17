#pragma once
#include "Http.hpp"

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
    void FillReq(const std::string& buff);
};
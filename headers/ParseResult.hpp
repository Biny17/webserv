#pragma once
#include "Http.hpp"

enum p_state {
    INIT,
    METHOD,
    PATH,
    QUERY,
    VERSION,
    HEAD_KEY,
    HEAD_VAL,
    BODY,
    CHUNK_SIZE,
    CHUNK_DATA,
    CHUNK_DATA_CRLF,
    COMPLETE,
    ERROR
};

class ParseResult {
    private:
        std::string cur_key;
        std::string cur_value;
        int content_length;
        p_state state;
        int pct;
        bool skip_leading_ws;
        
        void Error(std::string msg, int error_code);
        void Method(const std::string& buff, size_t& i);
        void Path(const std::string& buff, size_t& i);
        void Query(const std::string &buff, size_t& i);
        void Version(const std::string &buff, size_t& i);
        void HeadKey(const std::string &buff, size_t& i);
        void HeadValue(const std::string &buff, size_t& i);
        void AfterHeadersCheck();
        void Body(const std::string &buff, size_t& i);
        void Post();
        void TransferEncoding(const std::string &buff, size_t& i);

    public:
        HttpReq req;
        HttpError err;
        bool ok;
        void FillReq(const std::string& buff);
        ParseResult();
};
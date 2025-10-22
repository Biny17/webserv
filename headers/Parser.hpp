#pragma once
#include "Request.hpp"
#include "Response.hpp"

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

class Parser {
    private:
        static const unsigned int URI_MAX = 8192;
        static const unsigned int HEADER_MAX = 4096;

        std::string cur_key;
        std::string cur_value;
        std::string buff;
        int max_body_size;
        p_state state;
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
        void PostCheck();
        void TransferEncoding(const std::string &buff, size_t& i);

    public:
        Request& req;
        Response& err;
        bool ok;
        size_t FillReq(const std::string& buff);
        Parser(Request& request, Response& response);
        void Reset();
        void Print();
};

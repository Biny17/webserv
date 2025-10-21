#include "../headers/ParseResult.hpp"
#include "../headers/utils.hpp"
#include <sstream>
#include <iostream>

void ParseResult::Error(std::string msg, int error_code)
{
    state = ERROR;
    ok = false;
    if (msg == "")
        err.msg = "malformed request syntax";
    else
        err.msg = msg;
    err.error_code = error_code;
}

void ParseResult::Method(const std::string& buff, size_t& i)
{
    if (parse_token(buff, req.method, i) > 6) {
        Error("Unsupported method", 501);
        return ;
    }
    if (i == buff.length())
        return;
    if (buff[i] != ' ') {
        Error("", 400);
        return;
    }
    if (!valid_method(req.method)) {
        Error("Unsupported method", 501);
        return;
    }
    state = PATH;
    i++;
}

void ParseResult::Path(const std::string& buff, size_t& i)
{
    size_t start = i;
    while (i < buff.length() && is_abspath(buff[i]))
        i++;
    if ((req.path.length() + i - start) > 8192) {
        Error("Uri is too long >8192", 414);
        return;
    }
    req.path += buff.substr(start, i - start);
    if (i == buff.length())
        return;
    if ((buff[i] == ' ' || buff[i] == '?') && valid_path(req.path)) {
        state = (buff[i] == ' ') ? VERSION : QUERY;
        i++;
    }
    else
        Error("", 400);
}

void ParseResult::Query(const std::string &buff, size_t& i)
{
    size_t start = i;
    while (i < buff.length() && is_query(buff[i]))
        i++;
    req.query += buff.substr(start, i - start);
    if (i == buff.length())
        return;
    if (buff[i] == ' ') {
        state = VERSION;
        i++;
    }
    else
        Error("", 400);
}

void ParseResult::Version(const std::string &buff, size_t& i)
{
    size_t n = i;
    while (n < buff.length()
        && (std::isalnum(buff[n])
        || buff[n] == '/' || buff[n] == '.' ))
        n++;
    req.version += buff.substr(i, n - i);
    if (req.version != "HTTP/1.1" && req.version != "HTTP/1.0") {
        Error("Unsupported HTTP version", 505);
        return;
    }
    if (n+1 < buff.length() && buff[n] == '\r' && buff[n+1] == '\n') {
        state = HEAD_KEY;
        i = n+2;
    }
    else
        Error("", 400);
}

void ParseResult::HeadKey(const std::string &buff, size_t& i)
{
    if (buff[i] == '\r' && i+1 < buff.length() && buff[i+1] == '\n') {
        state = BODY;
        i += 2;
        AfterHeadersCheck();
        return;
    }
    size_t start = i;
    while (i < buff.length() && is_token(buff[i]))
        i++;
    cur_key += buff.substr(start, i - start);
    if (i == buff.length())
        return;
    if (buff[i] != ':') {
        Error("", 400);
        return;
    }
    state = HEAD_VAL;
    skip_leading_ws = true;
    i++;
}

void ParseResult::HeadValue(const std::string &buff, size_t& i)
{
    size_t start = i;

    if (skip_leading_ws) {
        while (i < buff.length() && (buff[i] == ' ' || buff[i] == '\t'))
            i++;
        start = i;
        if (i < buff.length())
            skip_leading_ws = false;
    }

    size_t crfl = buff.find("\r\n", i);
    if (crfl == std::string::npos) {
        cur_value += buff.substr(start, i - start);
        i = buff.length();
        return;
    }
    else {
        cur_value += buff.substr(start, crfl - start);
        req.headers[cur_key] = cur_value;
        cur_key.clear();
        cur_value.clear();
        state = HEAD_KEY;
        i = crfl + 2;
    }
}

void ParseResult::AfterHeadersCheck()
{
    if (req.headers.find("Host") != req.headers.end()) {
        Error("Host header is required", 400);
        return;
    }
    if (req.method == "GET")
    {
        if (req.headers.find("Content-Length") != req.headers.end() ||
            req.headers.find("Transfer-Encoding") != req.headers.end()) {
            Error("GET request must not have a body", 400);
            return;
        }
    }
    else if (req.method == "POST") {
        Post();
    }
}

void ParseResult::Post()
{
    std::map<std::string,std::string>::iterator te = req.headers.find("Transfer-Encoding");
    if (te != req.headers.end())
    {
        if (te->second != "chunked") {
            Error("Unsupported Transfer-Encoding", 501);
            return;
        }
        else {
            state = CHUNK_SIZE;
            return;
        }
    }
    std::map<std::string,std::string>::iterator ce = req.headers.find("Content-Length");
    if (ce != req.headers.end())
    {
        std::stringstream ss(ce->second);
        ss >> content_length;
        if (ss.fail() || !ss.eof() || ss.bad() || content_length < 0){
            Error("Invalid Content-Length", 400);
            return;
        }
    }
    else {
        Error("POST request must have a body", 400);
        return;
    }
}

void ParseResult::FillReq(const std::string& buff)
{

    size_t i = 0;
    if (state == ERROR)
        return;
    if (state == INIT)
        state = METHOD;
    if (state == METHOD && i < buff.length())
        Method(buff, i);
    if (state == PATH && i < buff.length())
        Path(buff, i);
    if (state == QUERY && i < buff.length())
        Query(buff, i);
    if (state == VERSION && i < buff.length())
        Version(buff, i);
    if (state == HEAD_KEY && i < buff.length())
        HeadKey(buff, i);
    if (state == HEAD_VAL && i < buff.length())
        HeadValue(buff, i);
}
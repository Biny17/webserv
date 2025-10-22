#include "../headers/ParseResult.hpp"
#include "../headers/utils.hpp"
#include <sstream>
#include <iostream>
#include <stdio.h>

ParseResult::ParseResult(): state(INIT), skip_leading_ws(true), ok(true), max_body_size(16384) {}

void ParseResult::Reset()
{
    state = INIT;
    ok = true;
    skip_leading_ws = true;
    cur_key.clear();
    cur_value.clear();
    req.content_len = 0;
    req.headers.clear();
    req.method.clear();
    req.query.clear();
    req.version.clear();
    req.path.clear();
}

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
    if (parse_token(buff, req.method, i, 15) > 6) {
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
    if ((req.path.length() + i - start) > URI_MAX) {
        Error("Uri is too long", 414);
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
    while (i < buff.length() && is_query(buff[i]) 
        && (req.path.length() + req.query.length() + i - start) < URI_MAX+1)
        i++;
    req.query += buff.substr(start, i - start);
    if (req.query.length()+req.path.length() > URI_MAX)
        return Error("Uri is too long", 414);
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
    size_t start = i;
    while (i < buff.length()
        && (std::isalnum(buff[i]) || buff[i] == '/' || buff[i] == '.' )
        && (i-start < 10))
        i++;
    req.version += buff.substr(start, i-start);
    if (req.version != "HTTP/1.1" && req.version != "HTTP/1.0")
        return Error("Unsupported HTTP version", 505);
    if (i+1 < buff.length() && buff[i] == '\r' && buff[i+1] == '\n') {
        state = HEAD_KEY;
        i = i+2;
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
    if ((i-start + cur_key.length()) > HEADER_MAX)
        return Error("Request Header Fields Too Large", 431);
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
        if (cur_value.length() + (i - buff.length()) > HEADER_MAX)
            return Error("Request Header Fields Too Large", 431);
        cur_value += buff.substr(start, i - buff.length());
        i = buff.length();
        return;
    }
    else {
        cur_value += buff.substr(start, crfl - start);
        cur_value = cur_value.substr(0, cur_value.find_last_not_of(" \t"));
        req.headers[cur_key] = cur_value;
        if (req.headers.size() > 10)
            return Error("Too many headers", 431);
        cur_key.clear();
        cur_value.clear();
        state = HEAD_KEY;
        i = crfl + 2;
    }
}

void ParseResult::AfterHeadersCheck()
{
    if (req.headers.find("Host") != req.headers.end())
        return Error("Host header is required", 400);
    if (req.method == "GET")
    {
        if (req.headers.find("Content-Length") != req.headers.end() ||
            req.headers.find("Transfer-Encoding") != req.headers.end())
            return Error("GET request must not have a body", 400);
    }
    else if (req.method == "POST") {
        PostCheck();
    }
}

void ParseResult::PostCheck()
{
    std::map<std::string,std::string>::iterator te = req.headers.find("Transfer-Encoding");
    if (te != req.headers.end())
    {
        if (te->second != "chunked")
            return Error("Unsupported Transfer-Encoding", 501);
        else {
            state = CHUNK_SIZE;
            return;
        }
    }
    std::map<std::string,std::string>::iterator ce = req.headers.find("Content-Length");
    if (ce != req.headers.end())
    {
        std::stringstream ss(ce->second);
        ss >> req.content_len;
        if (ss.fail() || !ss.eof() || ss.bad() || req.content_len < 0)
            return Error("Invalid Content-Length", 400);
        if (req.content_len > max_body_size)
            return Error("Payload Too Large", 413);
    }
    else
        return Error("POST request must have a body", 400);
}

size_t ParseResult::FillReq(const std::string& buff)
{

    size_t i = 0;
    if (state == ERROR)
        return i;
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
    while ((state == HEAD_KEY || state == HEAD_VAL) && i < buff.length())
    {
        if (state == HEAD_KEY)
           HeadKey(buff, i);
        if (state == HEAD_VAL && i < buff.length())
            HeadValue(buff, i);
    }
    return i;
}
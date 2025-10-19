#include "../headers/ParseResult.hpp"
#include "../headers/utils.hpp"

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
    size_t n = i;
    while (n < buff.length() && is_abspath(buff[n]))
        n++;
    req.path += buff.substr(i, n - i);
    if (n == buff.length())
        return;
    if ((buff[n] == ' ' || buff[n] == '?') && valid_path(req.path)) {        
        state = (buff[n] == ' ') ? VERSION : QUERY;
        i = n + 1;
    }
    else
        Error("", 400);
}

void ParseResult::Query(const std::string &buff, size_t& i)
{
    size_t n = i;
    while (n < buff.length() && is_query(buff[n]))
        n++;
    req.query += buff.substr(i, n - i);
    if (n == buff.length())
        return;
    if (buff[n] == ' ') {
        state = VERSION;
        i = n + 1;
    }
    else
        Error("", 400);
}

void ParseResult::Version(const std::string &buff, size_t& i)
{
    size_t n = i;
    while (n < buff.length() && (std::isalnum(buff[n]) || buff[n] == '/' || buff[n] == '.' ))
        n++;
    req.version += buff.substr(i, n - i);
    if (req.version != "HTTP/1.1" && req.version != "HTTP/1.0") {
        Error("Unsupported HTTP version", 505);
        return;
    }
    if (n+1 < buff.length() && buff[n] == '\r' && buff[n+1] == '\n') {
        state = HEADERS;
        i = n+2;
    }
    else
        Error("", 400);
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
    // if (state == QUERY && i < buff.length())

}
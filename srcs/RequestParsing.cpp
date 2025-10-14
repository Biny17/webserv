#include "../headers/Http.hpp"
#include "../headers/Parsing.hpp"

bool ParseToken(const std::string& src, std::string& dest, size_t &start)
{
    size_t n = start;
    while (n < src.length() && is_token(src[n]))
        n++;
    dest = src.substr(start, n-start);
    return (n-start > 0);
}

void Method(HttpReq&req, const std::string& buff, size_t& i)
{
    if (!ParseToken(buff, req.method, i))
        req.state = ERROR_STATE; // todo error struct
        
}

void FillReq(HttpReq& req, const std::string& buff)
{
    size_t i = 0;
    if (req.state == p_state::INIT)
        req.state = METHOD;
    if (req.state == p_state::METHOD)
        Method(req, buff, i);
}
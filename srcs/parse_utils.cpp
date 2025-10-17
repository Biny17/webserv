#include "../headers/utils.hpp"
#include <cstring>

inline bool is_req(unsigned char c)
{
    if (c > 0x20 && c < 0x7E)
        return true;
    return false;
}
inline bool is_token(unsigned char c)
{
    return std::isalnum(c) || strchr("!#$%&'*+-.^_`|~", c);
}

void ParseToken(const std::string& src, std::string& dest, size_t &start)
{
    size_t n = start;
    while (n < src.length() && is_token(src[n]))
        n++;
    dest += src.substr(start, n-start);
}

inline bool is_pchar(unsigned char c)
{
    return std::isalnum(c) || (strchr("-._~%!$&'()*+,;=", c) != 0);
}
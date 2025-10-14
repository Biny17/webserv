#include "../headers/Parsing.hpp"
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



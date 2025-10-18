#include "../headers/utils.hpp"
#include <cstring>

inline bool is_req(unsigned char c)
{
    if (c > 0x20 && c < 0x7E)
        return true;
    return false;
}
inline bool is_token(unsigned char c) {
    return std::isalnum(c) || strchr("!#$%&'*+-.^_`|~", c);
}

int parse_token(const std::string& src, std::string& dest, size_t &start)
{
    size_t n = start;
    while (n < src.length() && is_token(src[n]))
        n++;
    dest += src.substr(start, n-start);
    return (n - start);
}

inline bool is_pchar(unsigned char c) {
    return std::isalnum(c) || (strchr("-._~%!$&'()*+,;=", c) != 0);
}

inline bool is_abspath(unsigned char c) {
    return is_pchar(c) || (c == '/');
}

inline bool valid_method(const std::string& method) {
    return (method == "GET" || method == "POST" || method == "DELETE");
}

bool is_validpath(const std::string& path) {
    if (path[0] != '/')
        return false;
    size_t start = 1;
    while (start < path.length()) {
        size_t end = path.find('/', start);
        if (end == std::string::npos)
            end = path.length();
        if (end == start)
            return false;
        start = end + 1;
    }
    return true;
}
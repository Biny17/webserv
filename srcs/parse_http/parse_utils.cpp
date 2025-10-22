#include "../headers/utils.hpp"
#include <cstring>
#include <stdio.h>

bool is_req(unsigned char c)
{
    if (c > 0x20 && c < 0x7E)
        return true;
    return false;
}
bool is_token(unsigned char c) {
    return std::isalnum(c) || strchr("!#$%&'*+-.^_`|~", c);
}

bool is_xdigit(unsigned char c) {
    return std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
bool is_unreserved(unsigned char c) {
    return std::isalnum(c) || (strchr("-._~", c) != 0);
}

bool is_gen_delims(unsigned char c) {
    return (strchr(":/?#[]@", c) != 0);
}

bool is_sub_delims(unsigned char c) {
    return (strchr("!$&'()*+,;=", c) != 0);
}

bool is_pchar(unsigned char c) {
    return is_unreserved(c) || is_sub_delims(c) || (c == ':') || (c == '@') || (c == '%');
}

bool is_query(unsigned char c) {
    return is_pchar(c) || (strchr("/?", c) != 0);
}

bool is_abspath(unsigned char c) {
    return is_pchar(c) || (c == '/');
}

bool valid_method(const std::string& method) {
    return (method == "GET" || method == "POST" || method == "DELETE");
}

int parse_token(const std::string& src, std::string& dest, size_t &i, size_t limit)
{
    size_t start = i;
    while (i < src.length() && is_token(src[i]) && (i-start) < limit)
        i++;
    dest += src.substr(start, i-start);
    return (start - i);
}

bool valid_pct_encoded(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            if (i + 2 >= str.length() || !is_xdigit(str[i + 1]) || !is_xdigit(str[i + 2]))
                return false;
            i += 2;
        }
    }
    return true;
}

bool valid_path(const std::string& path)
{
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
    return valid_pct_encoded(path);
}

bool valid_h_key(const std::string &key)
{
    for (size_t i = 0; i < key.length(); i++) {
        if (!is_token(key[i]))
            return false;
    }
    return true;
}

bool valid_h_value(const std::string &value)
{
    for (size_t i = 0; i < value.length(); i++) {
        if (value[i] != '\t' && (value[i] < ' ' || value[i] > '~'))
            return false;
    }
    return true;
}
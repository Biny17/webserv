#pragma once
#include <string>

inline bool is_req(unsigned char c);
inline bool is_token(unsigned char c);
void parse_token(const std::string& src, std::string& dest, size_t &start);
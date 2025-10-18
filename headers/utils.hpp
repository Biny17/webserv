#pragma once
#include <string>

inline bool is_req(unsigned char c);
inline bool is_token(unsigned char c);
int parse_token(const std::string& src, std::string& dest, size_t &start);
inline bool is_pchar(unsigned char c);
inline bool valid_method(const std::string& method);
inline bool is_abspath(unsigned char c);
bool is_validpath(const std::string& path);
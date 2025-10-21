#pragma once
#include <string>

bool is_req(unsigned char c);
bool is_token(unsigned char c);
int parse_token(const std::string& src, std::string& dest, size_t &start);
bool is_pchar(unsigned char c);
bool valid_method(const std::string& method);
bool is_abspath(unsigned char c);
bool valid_path(const std::string& path);
bool is_query(unsigned char c);


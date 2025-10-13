#pragma once
#include "HTTP.hpp"
#include <chrono>

enum ClientState {
    RD_HEADER,
    RD_BODY,
    WRITING
};

struct Client {
    ClientState state;
    int socker_fd;
    std::chrono::time_point<std::chrono::system_clock> ts;

    std::string read_buff;
    size_t rdi;
    std::string write_buff;
    size_t wdi;

};
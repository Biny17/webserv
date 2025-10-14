#pragma once
#include "Http.hpp"
#include <chrono>

enum ClientState {
    READING,
    WRITE,
    CLOSED
};

struct Client {
    int fd;
    ClientState state;
    std::chrono::time_point<std::chrono::system_clock> ts;
    std::string write_buff;
    std::string tmp;
    size_t wdi;
    HttpReq request;
    bool keep_alive;

    Client(int socket_fd);
    void Parse(const std::string& buff);
};

#include "../headers/Client.hpp"

Client::Client(int socket_fd)
: state(READING), keep_alive(false), fd(socket_fd)
{
    ts = std::chrono::system_clock::now();
}

void Client::Parse(const std::string& buff)
{
    FillReq(this->request, buff);
}


#pragma once
#include <string>

struct Response {

    // Location *location;
    //enought for GET -----
    int         code;
    std::string status_message;
    int         content_length;
    std::string content_type;
    std::string msg;
    //---------------------
};
#pragma once
#include <string>

struct Response {

    //enought for GET -----
    int error_code;
    int content_length;
    std::string content_type;
    std::string msg;
    //---------------------

    Response();
};

#include "../../headers/Parser.hpp"
#include <iostream>
#include <map>
#include <utility>

void Parser::Print()
{
    if (ok)
    {
        std::cerr << "method: " << req.method << std::endl;
        std::cerr << "path: " << req.path << std::endl;
        std::cerr << "query: " << req.query << std::endl;
        std::cerr << "version: " << req.version << std::endl;
        std::map<std::string, std::string>::iterator it = req.headers.begin();
        std::cerr << "--------HEADERS----------" << std::endl;
        while (it != req.headers.end()) {
            std::cerr << it->first << ": " << it->second << std::endl;
            it++;
        }
    }
    else
    {
        std::cerr << "Error !" << std::endl;
        std::cerr << err.error_code << std::endl;
        std::cerr << err.msg << std::endl;
    }
}


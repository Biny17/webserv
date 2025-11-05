#include "webserv.hpp"

void Parser::Print()
{
    std::cout << std::endl << COLOR_BLUE << "------- REQUEST -------" << std::endl;
    if (ok)
    {
        std::cout << "method: " << req.method << std::endl;
        std::cout << "path: " << req.path << std::endl;
        std::cout << "query: " << req.query << std::endl;
        std::cout << "version: " << req.version << std::endl;
        std::map<std::string, std::string>::iterator it = req.headers.begin();
        while (it != req.headers.end()) {
            std::cout << it->first << ": " << it->second << std::endl;
            it++;
        }
    }
    else
    {
        std::cout << "Error !" << std::endl;
        std::cout << err.code << std::endl;
        std::cout << err.body << std::endl;
    }
    std::cout << COLOR_NC;
    std::cout << std::endl;
}

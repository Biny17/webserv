#include "webserv.hpp"

void Parser::Print()
{
    std::cerr << std::endl << COLOR_BLUE << "------- REQUEST -------" << std::endl;
    if (ok)
    {
        std::cerr << "method: " << req.method << std::endl;
        std::cerr << "path: " << req.path << std::endl;
        std::cerr << "query: " << req.query << std::endl;
        std::cerr << "version: " << req.version << std::endl;
        std::map<std::string, std::string>::iterator it = req.headers.begin();
        while (it != req.headers.end()) {
            std::cerr << it->first << ": " << it->second << std::endl;
            it++;
        }
    }
    else
    {
        std::cerr << "Error !" << std::endl;
        std::cerr << err.code << std::endl;
        std::cerr << err.body << std::endl;
    }
    std::cout << COLOR_NC;
}

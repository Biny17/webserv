#include "../../headers/webserv.hpp"

bool valid_filename(std::string& filename)
{
    if (filename.empty())
        return false;
    const std::string invalid_chars = "/\\?%*:|\"<>~";
    for (size_t i = 0; i < filename.length(); ++i) {
        if (invalid_chars.find(filename[i]) != std::string::npos) {
            return false;
        }
    }
    if (filename.find("..") != std::string::npos)
        return false;
    return true;
}
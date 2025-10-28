#include <string>

std::string get_extension(std::string& filename)
{
    std::string extension;
    size_t  point;

    point = filename.find_last_of('.');
    if (point != std::string::npos) {
        extension = filename.substr(point);
    }
    return extension;
}
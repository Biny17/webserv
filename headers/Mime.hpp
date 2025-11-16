#include <map>
#include <string>

class Mime
{
public:
    static std::string GetExtension(std::string mime_type);
    static std::string GetType(std::string extension);

private:
    static const std::map<std::string, std::string> mimeMap;
};
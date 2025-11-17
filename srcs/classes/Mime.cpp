#include <map>
#include <string>
#include "../../headers/Mime.hpp"

static std::map<std::string, std::string> createMimeMap()
{
    std::map<std::string, std::string> m;
    m.insert(std::make_pair(std::string("text/plain"), std::string(".txt")));
    m.insert(std::make_pair(std::string("text/html"), std::string(".html")));
    m.insert(std::make_pair(std::string("text/css"), std::string(".css")));
    m.insert(std::make_pair(std::string("text/javascript"), std::string(".js")));
    m.insert(std::make_pair(std::string("image/jpeg"), std::string(".jpg")));
    m.insert(std::make_pair(std::string("image/png"), std::string(".png")));
    m.insert(std::make_pair(std::string("image/gif"), std::string(".gif")));
    m.insert(std::make_pair(std::string("application/json"), std::string(".json")));
    m.insert(std::make_pair(std::string("application/xml"), std::string(".xml")));
    m.insert(std::make_pair(std::string("application/pdf"), std::string(".pdf")));
    m.insert(std::make_pair(std::string("application/zip"), std::string(".zip")));
    m.insert(std::make_pair(std::string("audio/mpeg"), std::string(".mp3")));
    m.insert(std::make_pair(std::string("video/mp4"), std::string(".mp4")));
    return m;
}

const std::map<std::string, std::string> Mime::mimeMap = createMimeMap();

std::string Mime::GetExtension(std::string mime_type)
{
    std::map<std::string, std::string>::const_iterator it = mimeMap.find(mime_type);
    if (it != mimeMap.end()) {
        return it->second;
    }
    return "";
}

std::string Mime::GetType(std::string extension)
{
    for (std::map<std::string, std::string>::const_iterator it = mimeMap.begin();
            it != mimeMap.end(); ++it) {
        if (it->second == extension)
            return it->first;
    }
    return "";
}


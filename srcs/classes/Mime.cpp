#include <map>
#include <string>
#include "../../headers/Mime.hpp"

const std::map<std::string, std::string> Mime::mimeMap = {
    {"text/plain", ".txt"},
    {"text/html", ".html"},
    {"text/css", ".css"},
    {"text/javascript", ".js"},
    {"image/jpeg", ".jpg"},
    {"image/png", ".png"},
    {"image/gif", ".gif"},
    {"application/json", ".json"},
    {"application/xml", ".xml"},
    {"application/pdf", ".pdf"},
    {"application/zip", ".zip"},
    {"audio/mpeg", ".mp3"},
    {"video/mp4", ".mp4"}
};

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
    return "application/octet-stream";
}


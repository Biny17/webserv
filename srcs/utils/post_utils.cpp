#include "../../headers/webserv.hpp"
#include <cerrno>
#include <cstring>

bool valid_filename(std::string& filename)
{
    if (filename.empty())
        return false;
    const std::string invalid_chars = "/\\?%*:|\"<>~";
    for (size_t i = 0; i < filename.length(); ++i) {
        if (invalid_chars.find(filename[i]) != std::string::npos) {
            std::cout << "invalid char |" << filename[i] << "|" << std::endl;
            return false;
        }
    }
    if (filename.find("..") != std::string::npos)
        return false;
    return true;
}

bool extract_boundary(const std::string& body, size_t& cur, const std::string& boundary)
{
    std::cout << COLOR_LIGHT_RED;
    if (body.compare(cur, 2, "--") != 0)
    {
        std::cout << "\nexpected '--' got: " << body.substr(cur, 20)
        << " at: " << cur << std::endl;
		return false;
    }
	cur += 2;
    // std::cout << COLOR_YELLOW << body.substr(cur) << COLOR_NC << std::endl;
    if (body.compare(cur, boundary.size(), boundary) != 0)
    {
        std::cout << "\nexpected boundary got: " << body.substr(cur, boundary.size()) << std::endl;
        return false;
    }
    cur += boundary.size();
    std::cout << COLOR_NC;
    return true;
}

bool validate_headers(const std::string& body, size_t& cur, size_t& header_end) {
    header_end = body.find("\r\n\r\n", cur);
    std::string tmp = body.substr(cur, header_end-cur);
    size_t content_disposition = tmp.find("Content-Disposition: form-data");
    if (header_end == std::string::npos || content_disposition == std::string::npos)
        return false;
    return true;
}

std::string extract_filename(const std::string& body, size_t& cur) {
    size_t fname_i = body.find("filename=\"", cur) + 10;
    if (fname_i == std::string::npos)
        return "";
    size_t closing_quote_i = body.find_first_of('"', fname_i);
    if (closing_quote_i == std::string::npos)
        return "";
    return body.substr(fname_i, closing_quote_i - fname_i);
}

bool write_file(const std::string& filename, const std::string& body, size_t cur, size_t data_end) {
    std::ofstream newfile(filename.c_str());
    if (!newfile) {
        std::cout << "Failed to open file: " << filename << std::endl;
        std::cout << "Error: " << std::strerror(errno) << std::endl;
        return false;
    }
    newfile << body.substr(cur, data_end - cur);
    return true;
}
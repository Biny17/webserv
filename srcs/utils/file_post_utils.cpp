#include "../../headers/webserv.hpp"

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

bool extract_boundary(const std::string& body, size_t& cur, const std::string& bnd) {
    if (body.compare(cur, bnd.size(), bnd) != 0) {
        return false;
    }
    cur += bnd.size();
    return true;
}

bool validate_headers(const std::string& body, size_t& cur, size_t& header_end) {
    header_end = body.find("\r\n\r\n", cur);
    if (header_end == std::string::npos
        || body.find("Content-Disposition: form-data", cur, header_end-cur)
            == std::string::npos)
    {
        return false;
    }
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

bool write_file(const std::string& filename, const std::string& body, size_t& cur, size_t data_end) {
    std::ofstream newfile(filename.c_str());
    if (!newfile) {
        return false;
    }
    newfile << body.substr(cur, data_end - cur);
    return true;
}
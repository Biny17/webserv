#include "webserv.hpp"
#include <iomanip>

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


void print_hex_string(const std::string& str) {
    // Save the current formatting state to restore it later
    std::ios oldState(NULL);
    oldState.copyfmt(std::cout);

    std::cout << "Hex Representation: ";

    // Set the stream for two-digit hex output
    std::cout << std::hex // Print integers in hexadecimal
              << std::uppercase // Use uppercase letters (A-F)
              << std::setfill('0'); // Pad with '0' to ensure two digits

    for (std::string::size_type i = 0; i < str.length(); ++i) {
        // Cast the character to an integer type (unsigned int or int)
        // to get its numeric value instead of printing it as a character.
        // The static_cast<unsigned int> ensures the value is treated as a positive number.
        std::cout << std::setw(2)
                  << static_cast<unsigned int>(static_cast<unsigned char>(str[i]))
                  << " "; // Add a space for separation
    }

    // Restore the original formatting state
    std::cout.copyfmt(oldState);

    std::cout << std::endl;
}

void print_location_info(Location& location)
{
    std::cout << COLOR_LIGHT_GREEN << std::endl;
    std::cout << "Location path: " << location.path << std::endl;
    std::cout << "root: " << location.root << std::endl;
    std::cout << "methods: ";
    for (size_t i = 0; i < location.methods.size(); i++)
    {
        std::cout << location.methods[i] << " ";
    }
    std::cout << std::endl;
    std::cout << COLOR_NC << std::endl;
}
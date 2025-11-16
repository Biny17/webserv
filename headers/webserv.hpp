#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <map>
# include <vector>
# include <stdint.h>
# include <unistd.h>
# include <csignal>
# include <cstdio>
# include <cstdlib>
# include <ctime>
# include <sys/types.h>
# include <sys/wait.h>
# include <dirent.h>
# include <fcntl.h>
# include <cstring>
# include <fstream>
# include <sstream>
# include <algorithm>
# include "Timeout.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Parser.hpp"
# include "Location.hpp"
# include "Client.hpp"
# include "Server.hpp"
# include "network.hpp"
# include "cgi.hpp"
# include "color.hpp"

// Global because I love the 42norm
extern std::vector<Server> servers;

// Utils
extern volatile sig_atomic_t shutdown_serv;
void		handle_shutdown(int sig);
std::string	autoindex(std::string const & directory, Request const &request);
int			check_allowed_methods(Server const &server, std::string const &req_path, std::string const &req_method, Request &request);
bool        valid_filename(std::string& filename);
int			content_type(std::string const &path);
bool        extract_boundary(const std::string& body, size_t& i, const std::string& bnd);
bool        validate_headers(const std::string& body, size_t& i, size_t& header_end);
std::string extract_filename(const std::string& body, size_t& i);
bool        write_file(const std::string& filename, const std::string& body, size_t& i, size_t data_end);
void        print_hex_string(const std::string& str);
std::string&	decode(std::string& str);

#endif // WEBSERV_HPP

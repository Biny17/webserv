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
std::string	autoindex(std::string const & directory);
int			check_allowed_methods(Server const &server, std::string const &req_path, std::string const &req_method, Request &request);
std::string get_extension(std::string& filename);

#endif // WEBSERV_HPP

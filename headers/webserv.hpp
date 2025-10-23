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
# include <sys/types.h>
# include <sys/wait.h>
# include <dirent.h>
# include <cstring>
# include <fstream>
# include <algorithm>
# include "Request.hpp"
# include "Parser.hpp"
# include "Location.hpp"
# include "Client.hpp"
# include "Server.hpp"
# include "network.hpp"
# include "cgi.hpp"

// Global because I love the 42norm
extern std::vector<Server> servers;

// Utils
extern volatile sig_atomic_t shutdown_serv;
void		handle_shutdown(int sig);
std::string	autoindex(std::string const & directory);

#endif // WEBSERV_HPP

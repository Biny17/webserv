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
# include <cstring>
# include <fstream>
# include <algorithm>
# include "Location.hpp"
# include "Client.hpp"
# include "Server.hpp"
# include "network.hpp"
# include "cgi.hpp"

// Utils
extern volatile sig_atomic_t shutdown_serv;
void	handle_shutdown(int sig);

#endif // WEBSERV_HPP

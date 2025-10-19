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
# include <fstream>
# include "Location.hpp"
# include "Client.hpp"
# include "Server.hpp"
# include "network.hpp"

// Utils
extern volatile sig_atomic_t shutdown_serv;
void	handle_shutdown(int sig);

#endif // WEBSERV_HPP

#ifndef NETWORK_HPP
# define NETWORK_HPP

# include <iostream>
# include <cstdio>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/epoll.h>
# include <csignal>
# include "webserv.hpp"

# define LISTENING_PORT 8080
# define MAX_EVENTS 1024

void	launch_server(void);
void	accept_new_client(int epfd, int sockfd, std::map<int, int>& fd_map);
void	read_client_data(int epfd, int clifd, std::map<int, int>& fd_map);

extern volatile sig_atomic_t shutdown_serv;
void	handle_shutdown(int sig);

#endif // NETWORK_HPP

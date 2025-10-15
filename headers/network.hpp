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

# define MAX_EVENTS 1024
# define REQUEST_BUFF_SIZE 1024

struct Server;
struct Client;

void	launch_server(std::vector<Server>& servers);
void	accept_new_client(int epf, Server& server);
void	read_client_data(int epfd, int clifd, Server& server);

void	event_loop(int epfd, std::vector<Server>& servers);

extern volatile sig_atomic_t shutdown_serv;
void	handle_shutdown(int sig);

#endif // NETWORK_HPP

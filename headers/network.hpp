#ifndef NETWORK_HPP
# define NETWORK_HPP

# include <iostream>
# include <cstdio>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/epoll.h>
# include "config.hpp"

# define LISTENING_PORT 8080
# define MAX_EVENTS 1024

void	launch_server(void);
void	accept_new_client(int epfd, int sockfd);
void	read_client_data(int epfd, int clifd);

void	parse_conf(std::string filename, std::vector<Server> servers);

#endif // NETWORK_HPP

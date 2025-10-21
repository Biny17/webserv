#ifndef NETWORK_HPP
# define NETWORK_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/epoll.h>
# include "webserv.hpp"

# define MAX_EVENTS 1024
# define REQUEST_BUFF_SIZE 1024

class Client;
class Server;

void	launch_server(std::vector<Server>& servers);
void	event_loop(int epfd);
void	set_epoll_event(int epfd, int clifd, uint32_t event);
void	accept_new_client(int epfd, int sockfd, Server& server);
void	read_client_data(int epfd, int clifd, Server& server);
bool	send_response(int clifd, Server& server);
void	disconnect_client(int epfd, int clifd, Server& server);

#endif // NETWORK_HPP

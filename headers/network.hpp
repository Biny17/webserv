#ifndef NETWORK_HPP
# define NETWORK_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/epoll.h>
# include "webserv.hpp"

# define MAX_EVENTS 1024
# define REQUEST_BUFF_SIZE 8192
# define EPOLL_TIMEOUT 500
# define CLIENT_TIMEOUT 10.0
# define CGI_TIMEOUT 5.0

class Client;
class Server;

void	launch_server(std::vector<Server>& servers);
void	event_loop(int epfd);
void	set_epoll_event(int epfd, Client& client, uint32_t event);
void	accept_new_client(int sockfd, Server& server);
void	read_client_data(Client& client, Server& server);

//request func
void	handle_request(Server &server, Client &client, Request &request, Response &response);
void	get_static_file(Request const &request, Response &response);
void	build_delete_response(Server const &server, Request const &request, Response &response);

#endif // NETWORK_HPP

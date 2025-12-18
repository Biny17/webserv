#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"

int		launch_cgi(std::string &filename, Server& server, Client& client);
void	listen_cgi(Server& server, Client& client);
void	kill_cgi(Client& cgi);

std::string	get_available_cgi(Server& server);

#endif // CGI_HPP

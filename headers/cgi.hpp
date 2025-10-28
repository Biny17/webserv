#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"

void	add_cgi(Server& server, Client& client, std::string& filename);
int		launch_cgi(std::string &filename, Client& client, char **env);
void	listen_cgi(Server& server, Client& client);

std::string	get_available_cgi(void);

#endif // CGI_HPP

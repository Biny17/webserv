#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"

void	add_cgi(Server& server, Client& client, std::string& filename);
int		launch_cgi(std::string &filename, char **env);
void	listen_cgi(Server& server, Client& client);

#endif // CGI_HPP

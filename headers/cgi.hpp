#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"

void	add_cgi(Server& server, Client& client, std::string& filename);
int		launch_cgi(std::string &filename, char **env);
void	listen_cgi(Server& server, Client& client);

std::vector<std::string>	list_file_extension(std::string const &directory, std::string const &ext);

#endif // CGI_HPP

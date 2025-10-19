#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

class Client;
class Location;

class Server {

	public:
		Server(void);
		~Server(void);

		int							socket;
		std::string					server_name;
		std::vector<int>			listen;
		std::string					root;
		unsigned long				max_upload;
		std::vector<std::string>	index_page;
		std::vector<Location>		locations;
		std::map<int, std::string>	err_page;

		std::map<int, Client>	clients;

		bool	hasFD(int fd) const;
		void	addClient(int clifd);
		void	removeClient(int clifd);

};
std::ostream	&operator<<(std::ostream &o, Server const &serv);

void	parse_conf(std::string filename, std::vector<Server> &servers);

#endif // SERVER_HPP



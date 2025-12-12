#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

class Client;
class Location;

class Server {

	public:
		Server(void);
		~Server(void);

		std::vector<int>			sockets;
		std::string					server_name;
		std::vector<int>			listen;
		std::string					root;
		unsigned int				max_upload;
		std::vector<std::string>	index_page;
		std::vector<Location>		locations;
		std::map<int, std::string>	err_page;
		int							epfd;

		std::map<int, Client>	clients;

		bool	isSockFD(int fd) const;
		bool	hasFD(int fd) const;
		void	addClient(int clifd);
		void	removeClient(int clifd);

};
std::ostream	&operator<<(std::ostream &o, Server const &serv);

void	parse_conf(std::string filename, std::vector<Server> &servers);

#endif // SERVER_HPP

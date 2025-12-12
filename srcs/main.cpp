#include "webserv.hpp"

std::vector<Server>	servers;

void	print_servers(std::vector<Server> servers)
{
	std::vector<Server>::iterator	it;
	for (it  = servers.begin(); it != servers.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}

int	main(int ac, char **av)
{
	int epfd = -1;

	if (ac > 2)
	{
		std::cout << "Invalid amount of arguments, please launch the program with a config file" << std::endl;
		return (1);
	}
	std::string conf = "config/server.conf";
	if (ac == 2)
		conf = av[1];
	try {
		try {
			parse_conf(conf, servers);
		}
		catch(std::runtime_error& e) {
			std::cerr << "Parsing error: " << e.what() << '\n';
			return (1);
		}
		// print_servers(servers);
		launch_server(servers, epfd);
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		if (epfd >= 0)
			close(epfd);
		return (1);
	}
	return (0);
}

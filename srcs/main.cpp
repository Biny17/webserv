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
	if (ac != 2)
	{
		std::cout << "Invalid amount of arguments, please launch the program with a config file" << std::endl;
		return (1);
	}

	try {
		try {
			parse_conf(av[1], servers);
		}
		catch(std::runtime_error& e) {
			std::cerr << "Parsing error: " << e.what() << '\n';
			return (1);
		}
		// print_servers(servers);
		launch_server(servers);
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}

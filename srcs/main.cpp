#include "webserv.hpp"

std::vector<Server> parse_config(int ac, char** av)
{
	std::vector<Server> servers;
	(void)ac;
	(void)av;
	return (servers);
}

int	main(int ac, char **av)
{
	try {
		std::vector<Server> servers = parse_config(ac, av);
		launch_server(servers);
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
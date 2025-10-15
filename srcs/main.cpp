#include "webserv.hpp"

int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	try {
		// Get config file
		std::vector<Server> servers;
		launch_server(servers);
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
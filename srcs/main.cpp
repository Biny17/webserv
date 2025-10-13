#include "webserv.hpp"

int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	try {
		// Get config file
		launch_server();
	}
	catch (std::exception& e) {
		std::cout << "An error has occured: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
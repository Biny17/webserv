#include "webserv.hpp"

int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	std::vector<Server>	servers;
	try {
		parse_conf(av[1], servers);
	}
	catch(std::runtime_error& e) {
		std::cerr << "Error while parcing : " << e.what() << '\n';
		return (0);
	}
	std::vector<Server>::iterator	it = servers.begin();
	for (it = servers.begin(); it != servers.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
	
	// try {
	// 	// Get config file
	// 	launch_server();
	// }
	// catch (std::exception& e) {
	// 	std::cout << "An error has occured: " << e.what() << std::endl;
	// 	return (1);
	// }
	return (0);
}
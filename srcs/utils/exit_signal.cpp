#include "webserv.hpp"

volatile sig_atomic_t shutdown_serv = 0;

// SIGINT handler for shutdown
void	handle_shutdown(int sig)
{
	shutdown_serv = sig;
	std::cout << std::endl << "quitting..." << std::endl;
	return ;
}

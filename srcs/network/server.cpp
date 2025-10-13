#include "network.hpp"

void	launch_server(void)
{
	// Create IPv4 TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return ;
	}

	// Bind socket to port
	struct sockaddr_in addr;				// Address representation
	addr.sin_family = AF_INET;				// IPv4 addr
	addr.sin_port = htons(LISTENING_PORT);	// Set the port to listen on (htons convert to network endian)
	addr.sin_addr.s_addr = INADDR_ANY;		// Listen on all private network IPs
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return ;
	}
}
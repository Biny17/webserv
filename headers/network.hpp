#ifndef NETWORK_HPP
# define NETWORK_HPP

# include <iostream>
# include <cstdio>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>

# define LISTENING_PORT 8080

void	launch_server(void);

#endif // NETWORK_HPP
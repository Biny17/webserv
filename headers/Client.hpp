#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserv.hpp"

class Client {

	public:
		Client(void);
		~Client(void);

		int	fd;

};

#endif // CLIENT_HPP
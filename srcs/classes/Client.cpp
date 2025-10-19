#include "Client.hpp"

Client::Client(void)
{
	this->fd = -1;
}

Client::~Client(void)
{
	if (this->fd >= 0)
		close(this->fd);
}

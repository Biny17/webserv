#include "webserv.hpp"

void	Timeout::Start(void)
{
	this->enabled = true;
	this->Update();
}

void	Timeout::Stop(void)
{
	this->enabled = false;
}

void	Timeout::Update(void)
{
	this->time = std::time(0);
}

bool	Timeout::Enabled(void) const
{
	return (this->enabled);
}

bool	Timeout::Check(bool isCGI) const
{
	if (this->enabled == false)
		return (false);

	std::time_t	now = std::time(0);
	double		seconds = std::difftime(now, this->time);

	if (isCGI)
		return (seconds >= CGI_TIMEOUT);
	return (seconds >= CLIENT_TIMEOUT);
}

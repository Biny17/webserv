#ifndef TIMEOUT_HPP
# define TIMEOUT_HPP

# include "webserv.hpp"

class Timeout
{
	private:
		std::time_t	time;
		bool		enabled;

	public:
		void	Start(void);
		void	Stop(void);
		void	Update(void);
		bool	Enabled(void) const;
		bool	Check(void) const;

};

#endif // TIMEOUT_HPP

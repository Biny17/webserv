#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"

class Location {

	public:
		Location(void);
		~Location(void);

		int							autoindex;
		std::string					path;
		std::vector<std::string>	index;
		std::string					root;
		// std::string					redirect;
		std::string					cgi_path;
		std::string					cgi_extension;
		std::string					upload_dir;
		std::vector<std::string>	methods;
		// std::vector<Location>		locations;

};
std::ostream	&operator<<(std::ostream &o, Location const &loc);

#endif // !LOCATION_HPP

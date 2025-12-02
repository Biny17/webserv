#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"

class Location {

	public:
		Location(void);
		~Location(void);

		bool						autoindex;
		std::string					path;
		std::vector<std::string>	index;
		std::string					root;
		std::map<int, std::string>	redirect;
		std::string					cgi_path;
		std::vector<std::string>	cgi_extension;
		std::string					upload_dir;
		std::vector<std::string>	methods;
		std::map<std::string, std::string>	extension;
};
std::ostream	&operator<<(std::ostream &o, Location const &loc);

#endif // !LOCATION_HPP

#include "Location.hpp"

Location::Location(void)
{
	this->autoindex = 0;
	return ;
}

Location::~Location(void)
{
	return ;
}

std::ostream	&operator<<(std::ostream &o, Location const &loc) {

	std::cout << "++++++++++++++++++++++Location+++++++++++++++++++" << std::endl;
	if (!loc.path.empty())
		o << "path : " << loc.path << std::endl;
	if (!loc.root.empty())
		o << "root : " << loc.root << std::endl;
	o << "autoindex : " << loc.autoindex << std::endl;
	// if (!loc.redirect.empty())
	// 	o << "redirect : " << loc.path << std::endl;
	if (!loc.upload_dir.empty())
		o << "upload_dir : " << loc.path << std::endl;
	if (!loc.index.empty())
	{
		o << "index : ";
		for (std::vector<std::string>::const_iterator it = loc.index.begin(); it != loc.index.end(); ++it)
			o << *it << " ";
		o << std::endl;
	}
	if (!loc.cgi_path.empty())
		o << "cgi_path : " << loc.cgi_path << std::endl;
	if (!loc.extension.empty())
	{
		for (std::map<std::string, std::string>::const_iterator it = loc.extension.begin(); it != loc.extension.end(); ++it)
			o << it->first << ", " << it->second << std::endl;
	}
	if (!loc.methods.empty())
	{
		o << "methods : ";
		for (size_t i = 0; i < loc.methods.size(); i++)
		{
			o << loc.methods[i] << " ";
		}
		o << std::endl;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	return o;
}

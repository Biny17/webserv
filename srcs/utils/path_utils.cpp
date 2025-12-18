#include "webserv.hpp"

//check if the content given in path is a directory or a file,

Target::Type target_type(std::string const &path)
{
	if (access(path.c_str(), F_OK) == -1)		//return 0 if undefined
		return (Target::NOTFOUND);

	int fd = open(path.c_str(), O_DIRECTORY);	//return 2 if directory
	if (fd != -1)
	{
		std::cout << COLOR_BROWN << "It's a DIRECTORY ! " << COLOR_NC << std::endl;
		return (close(fd), Target::DIR);
	}

	fd = open(path.c_str(), O_RDONLY);			//return 1 if file
	if (fd != -1)
	{
		std::cout << COLOR_BROWN << "It's a FILE ! " << COLOR_NC << std::endl;
		return (close(fd), Target::FILE);
	}
	return (Target::ERROR);
}

std::string get_extension(const std::string& filename)
{
    std::string extension;
    size_t  point;

    point = filename.find_last_of('.');
    if (point != std::string::npos) {
        extension = filename.substr(point);
    }
    return extension;
}

std::string& trim_trailing_slash(std::string& path)
{
	path.erase(path.find_last_not_of('/')+1);
	return path;
}

std::string& add_trailing_slash(std::string& path)
{
	if (path.empty() || *(path.end()-1) != '/')
		path += '/';
	return path;
}

std::string& add_leading_slash(std::string& path)
{
	if (path.empty() || *(path.begin()) != '/')
		path = '/' + path;
	return path;
}

std::string path_add(std::string base, std::string addition)
{
	std::cout << COLOR_BROWN << "base: " << base <<
		" | addition: " << addition << COLOR_NC << std::endl;
	if (base.empty())
		return addition;
	if (addition.empty())
		return base;
	base = trim_trailing_slash(base);
	addition.erase(0, addition.find_first_not_of('/'));
	return base + "/" + addition;
}

bool	match_location(std::string &srcs, std::vector<Location> &locations)
{
	std::vector<Location>::iterator	it = locations.begin();
	std::vector<Location>::iterator	ite = locations.end();

	for (it = locations.begin(); it != ite; ++it) {
		if ((*it).path == srcs)
			return (true);
	}
	return (false);
}

Location &find_location(std::string &srcs, std::vector<Location> &locations)
{
	std::vector<Location>::iterator	it = locations.begin();
	std::vector<Location>::iterator	ite = locations.end();

	for (it = locations.begin(); it != ite; ++it)
	{
		if ((*it).path == srcs)
			return (*it);
	}
	return (*ite);
}

bool	is_readable_file(const std::string &path)
{
	struct stat statv;

	if (stat(path.c_str(), &statv) == -1)
		return false;
	if (!S_ISREG(statv.st_mode))
		return false;
	if (access(path.c_str(), R_OK) == -1)
		return false;
	return true;
}
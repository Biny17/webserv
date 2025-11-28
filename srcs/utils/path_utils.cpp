#include "webserv.hpp"

//check if the content given in path is a directory or a file,
//ret value :
//1 if file, 2 if directory, 0 if undefined
int		target_type(std::string const &path) {
	if (access(path.c_str(), F_OK) == -1)		//return 0 if undefined
		return (0);

	int fd = open(path.c_str(), O_DIRECTORY);	//return 2 if directory
	if (fd != -1)
		return (close(fd), 2);

	fd = open(path.c_str(), O_RDONLY);			//return 1 if file
	if (fd != -1)
		return (close(fd), 1);
	return (0);
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


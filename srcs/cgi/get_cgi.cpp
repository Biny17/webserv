#include "webserv.hpp"

std::vector<std::string>	list_file_extension(std::string const &directory, std::string const &ext) { //mettre le directory en entree ainsi que l'extension choisit en toute lettre exemple : python = ".py"
	DIR							*dir = opendir(directory.c_str());
	struct dirent				*content;
	std::vector<std::string>	result;

	if (!dir)
		return (result);	//renvoie vector vide pour l'instant
	content = readdir(dir);
	while (content)
	{
		std::string	file = content->d_name;
		if (file.find_last_of('.') != file.npos
			&& file.substr(file.find_last_of('.'), file.size()) == ext)
			result.push_back(file);
		content = readdir(dir);
	}
	closedir(dir);
	return (result);	//retourne un vecteur contenant tout le snom de fichier avec l'extenssion voulu
}

std::string	get_available_cgi(Server& server)
{
	std::vector<Location>::iterator	bin = find_location(server.cgi_path, server.locations);
	if (bin == server.locations.end())
		return ("[]");

	std::vector< std::vector<std::string> >	files;

	std::vector<std::string>::iterator		ext;
	std::vector<std::string>::iterator		extsEnd = bin->cgi_extension.end();

	for (ext = bin->cgi_extension.begin(); ext != extsEnd; ++ext)
		files.push_back(list_file_extension(bin->alias, *ext));

	std::string	result;
	result.append("[");

	std::vector< std::vector<std::string> >::iterator	fileIt;
	for (fileIt = files.begin(); fileIt != files.end(); ++fileIt)
	{
		for (ext = fileIt->begin(); ext != fileIt->end(); ++ext)
			result += "\"" + *ext + "\", ";
	}

	if (result.size() >= 2)
		result.erase(result.end() - 2, result.end());

	result.append("]");
	return (result);
}

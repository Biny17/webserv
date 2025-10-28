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

std::string	cgi_line(std::vector<std::string> const &ext1, std::vector<std::string> const &ext2) {

	std::string	result;

	std::vector<std::string>::const_iterator	it;
	std::vector<std::string>::const_iterator	ite = ext1.end();

	result.append("[");
	for(it = ext1.begin(); it != ite; ++it)
		result += "\"" + *it + "\", ";

	ite = ext2.end();
	for(it = ext2.begin(); it != ite; ++it)
		result += "\"" + *it + "\", ";
	if (result.size() >= 2)
		result.erase(result.end() - 2, result.end());
	result.append("]");
	return(result);
}

std::string	get_available_cgi(void)
{
	return (cgi_line(list_file_extension("cgi-bin", ".py"), list_file_extension("cgi-bin", ".sh")));
}

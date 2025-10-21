#include "webserv.hpp"

std::vector<std::string>	list_file_extension(std::string const &directory, std::string const &ext) { //mettre le directory en entree ainsi que l'extension choisit en toute lettre exemple : python = ".py"
	DIR							*dir = opendir(directory.c_str());
	struct dirent				*content;
	std::vector<std::string>	result;

	if (!dir)
		return (std::cout << "invalid opendir arg" << std::endl, result);	//renvoie vector vide pour l'instant
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
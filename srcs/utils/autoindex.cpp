#include "webserv.hpp"

std::string	autoindex(std::string const & directory) {	//preciser le directory
	DIR				*dir = opendir(directory.c_str());	//man opendir
	struct dirent	*content;
	std::string		answer;

	if (!dir)
		return (std::cout << "invalid opendir arg" << std::endl, "");	//renvoie string vide pour l'instant
	answer += "<!DOCTYPE html>\n" \
				"<html>\n" \
				"<head>\n" \
				"<meta charset='utf-8'>\n"
				"<title>";
	answer += "Index of " + directory + "/";
	answer += "</title>\n" \
				"<meta name='viewport' content='width=device-width, initial-scale=1'>\n" \
				"<link rel='stylesheet' type='text/css' media='screen' href='main.css'>\n" \
				"<script src='main.js'></script>\n" \
				"</head>\n" \
				"<body>\n";
	content = readdir(dir);
	while (content) {
		std::string	filename = content->d_name;
		if (*filename.begin() != '.'){
			answer += "<a href=\"" + filename + "\">" + filename + "</a>\n";
		}
		content = readdir(dir);		//a chaque appel de readdir on passe au fichier suivant
	}
	answer += "</body>\n" \
				"</html>";
	close(dir);
	return (answer);	//renvoie une string contennant la page html a afficher (ca reste une string) :)
}
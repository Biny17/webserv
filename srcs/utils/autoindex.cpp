#include "webserv.hpp"

std::string	autoindex(std::string const & directory, Request const &request) {	//preciser le directory
	std::cout << COLOR_GREEN << "opendir: " << directory.c_str() << std::endl;
	DIR				*dir = opendir(directory.c_str());	//man opendir
	struct dirent	*content;
	std::string		answer;

	if (!dir)
		return (std::cout << "invalid opendir arg" << std::endl, "");	//renvoie string vide pour l'instant
	answer += "<!DOCTYPE html>\n" \
				"<html>\n" \
				"<head>\n" \
				"<meta charset='UTF-8'>\n"
				"<title>";
	answer += "Index of " + directory + "/";
	answer += "</title>\n" \
				"<meta name='viewport' content='width=device-width, initial-scale=1'>\n" \
				"<style>\n" \
					"body {\n" \
					"font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, \"Helvetica Neue\", Arial, sans-serif;\n" \
					"background: #f5f7fa;\n" \
					"color: #222;\n" \
					"margin: 0;\n" \
					"padding: 24px;\n" \
				"}\n" \
"\n" \
				".container {\n" \
					"max-width: 900px;\n" \
					"margin: 0 auto;\n" \
				"}\n" \
"\n" \
				"h1 {\n" \
					"font-size: 1.25rem;\n" \
					"margin: 0 0 12px;\n" \
					"color: #111;\n" \
				"}\n" \
"\n" \
				"/* Links list: make each entry a simple row */\n" \
				"a {\n" \
					"display: block;\n" \
					"padding: 8px 12px;\n" \
					"margin: 4px 0;\n" \
					"color: #0b66c3;\n" \
					"text-decoration: none;\n" \
					"border-radius: 4px;\n" \
					"background: #ffffff;\n" \
					"border: 1px solid #e6e9ee;\n" \
					"transition: background 120ms ease, transform 120ms ease;\n" \
				"}\n" \
"\n" \
				"a:hover, a:focus {\n" \
					"background: #eef6ff;\n" \
					"transform: translateY(-1px);\n" \
					"outline: none;\n" \
				"}\n" \
"\n" \
				"/* Distinguish directories (if server appends a trailing slash) */\n" \
				"a[href$=\"/\"] {\n" \
					"font-weight: 600;\n" \
					"color: #0a4f9a;\n" \
				"}\n" \
"\n" \
				"/* Small footer spacing */\n" \
				"footer {\n" \
					"margin-top: 18px;\n" \
					"font-size: 0.9rem;\n" \
					"color: #666;\n" \
				"}\n" \
				"</style>" \
				"</head>\n" \
				"<body>\n";
	content = readdir(dir);
	while (content) {
		std::string	filename = content->d_name;
		if (*filename.begin() != '.'){
			std::string pathv2(request.path);
			if (*pathv2.rbegin() != '/')
				pathv2.push_back('/');
			answer += "<a href=\"" + pathv2 + filename + "\">" + filename + "</a>\n";
		}
		content = readdir(dir);		//a chaque appel de readdir on passe au fichier suivant
	}
	answer += "</body>\n" \
				"</html>";
	closedir(dir);
	return (answer);	//renvoie une string contennant la page html a afficher (ca reste une string) :)
}
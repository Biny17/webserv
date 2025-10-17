/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_parsing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:57:33 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/17 12:34:30 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"config.hpp"
#include<cstdlib>


//reproduit le fonctionnement de emplace_back() pour les vector
template <typename T>
static void construct_back(std::vector<T> &container) {
	T	thing;

	container.push_back(thing);
}

//remplie un bloc server
static void	parse_param(std::vector<std::string> const &words, Server &server) {

	std::vector<std::string>::const_iterator	it = words.begin();
	std::vector<std::string>::const_iterator	ite = words.end();
	int											size = (int)words.size();

	if (*it == "listen")
		for (it = words.begin() + 1; it != ite; ++it) {
			for (size_t i = 0; i < (*it).size(); i++)
				if (!std::isdigit((*it)[i]))
					throw std::runtime_error(*it + " argument error");
			server.listen.push_back(std::atoi((*it).c_str()));
		}
	else if (*it == "server_name") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		server.server_name = *(it + 1);
	}
	else if (*it == "index") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		server.index_page = *(it + 1);
	}
	else if (*it == "client_max_body_size") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		for (size_t i = 0; i < (*(it + 1)).size(); i++)
			if (!std::isdigit((*(it + 1))[i]))
				throw std::runtime_error(*it + " argument error");
		server.max_upload = std::atoi((*(it + 1)).c_str());
	}
	else if (*it == "root") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		server.root = *(it + 1);
	}
	else if (*it == "error_page") {
		if (size != 3)
			throw std::runtime_error(*it + " argument error");
		for (size_t i = 0; i < (*(it + 1)).size(); i++)
			if (!std::isdigit((*(it + 1))[i]))
				throw std::runtime_error(*it + " argument error");
		std::pair<int, std::string>	err(std::atoi((*(it + 1)).c_str()), *(it + 2));
		server.err_page.insert(err);
	}
	else
		throw std::runtime_error("server unknown command : " + *it);
}


//remplie un bloc location
static void	parse_param(std::vector<std::string> const &words, Location &location) {

	std::vector<std::string>::const_iterator	it = words.begin();
	std::vector<std::string>::const_iterator	ite = words.end();
	int											size = (int)words.size();

	if (*it == "autoindex") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		if (*(it + 1) == "on")
			location.autoindex = 1;
		else if (*(it + 1) == "off")
			location.autoindex = 0;
		else
			throw std::runtime_error(*it + " argument error");
	}
	else if (*it == "index") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		location.index = *(it + 1);
	}
	else if (*it == "root") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		location.root = *(it + 1);
	}
	else if (*it == "allow_methods")
		for (it = words.begin() + 1; it != ite; ++it) {
			if (*it != "GET" && *it != "POST" && *it != "DELETE")
				throw std::runtime_error(*it + " argument error");
			location.methods.push_back(*it);
		}
	else if (*it == "cgi_path") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		location.cgi_path = *(it + 1);
	}
	else if (*it == "cgi_extension") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		location.cgi_extension = *(it + 1);
	}
	else
		throw std::runtime_error("location unknown command : " + *it);
}

//point de passage de toute les ligne, sert a savoir si on entre dans un bloc ou si on quite un bloc, ou alors si on a un argument de bloc
static void	parse_line(std::vector<std::string> const &words, std::vector<Server> &servers, int &level) {

	std::vector<std::string>::const_iterator	it = words.begin();
	// static int									level = 0;			//sert a definir si a quel niveau on est : 1 = dans server, 2 = dans location, 0 = dans rien

	if (*it == "server") {
		if (level != 0 || words.size() != 2 || *(it + 1) != "{")
			throw std::runtime_error(*it + " bad line");
		construct_back<Server>(servers);
		level++;
	}
	else if (*it == "location") {
		if (level != 1 || words.size() != 3 || *(it + 2) != "{")
			throw std::runtime_error(*it + " bad line");
		construct_back<Location>(servers.back().locations);
		servers.back().locations.back().path = *(it + 1);
		level++;
	}
	else if (*it == "}") {
		if (level < 1)
			throw std::runtime_error("'}' without '{' before");
		level--;
	}
	else {
		if (level == 1)											//level = 1 -> on est dans un bloc server
			parse_param(words, servers.back());
		else if (level == 2)									//level = 2 -> on est dans un bloc locaton
			parse_param(words, servers.back().locations.back());
		else
			throw std::runtime_error("something wrong");		//si level ]1;2[ a ce moment la, alors erreur
	}
}

//fonction de parsing du fichier .conf
void	parse_conf(std::string filename, std::vector<Server> &servers) {

	std::ifstream				file(filename.c_str());
	std::string					line;
	int							level = 0;

	if (!file.is_open())
		throw std::runtime_error("invalid file or not right access");
	while(std::getline(file, line))											//parsing ligne par ligne
	{
		bool						endline = 0;
		std::vector<std::string>	words;									//vector de decoupe en mots
		std::string					buffer;
		for (std::string::iterator its = line.begin(); its != line.end(); ++its)
		{
			if (std::isspace(*its) || *its == ';' || *its == '#') {
				if (!buffer.empty())
					words.push_back(buffer);
				buffer.clear();
				if (*its == ';' || *its == '#')	{							//caractere d'arret de ligne
					endline = 1;
					break;
				}
			}
			else if (*its == '{' || *its == '}') {
				endline = 1;
				if (!buffer.empty())
					words.push_back(buffer);
				buffer.clear();
				std::string	temp(1, *its);
				words.push_back(temp);
			}
			else
				buffer.push_back(*its);
		}
		if (!buffer.empty())
			throw std::runtime_error(buffer + " : bad syntax or unknown commands");
		if (!words.empty())	{												//on ignore les ligne vide
			if (!endline)
				throw std::runtime_error("bad line");
			parse_line(words, servers, level);
		}
	}
	if (level != 0)
		throw std::runtime_error("{ not close error");
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_parsing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:57:33 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/16 15:27:13 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"config.hpp"
#include<cstdlib>

static void	parse_param(std::vector<std::string> const &words, Server &server) {

	std::vector<std::string>::const_iterator	it = words.begin();
	std::vector<std::string>::const_iterator	ite = words.end();
	int											size = (int)words.size();

	if (size <= 1)
		throw ;
	if (*it == "listen")
		for (it = words.begin() + 1; it != ite; ++it)
		{
			for (int i = 0; i < (*it).size(); i++)
				if (!std::isdigit((*it)[i]))
					throw ;
			server.listen.push_back(std::atoi((*it).c_str()));
		}
	else if (*it == "server_name") {
		if (size != 2)
			throw ;
		server.server_name = *(it + 1);
	}
	else if (*it == "index") {
		if (size != 2)
			throw ;
		server.index_page = *(it + 1);
	}
	else if (*it == "client_max_body_size") {
		if (size != 2)
			throw ;
		for (int i = 0; i < (*(it + 1)).size(); i++)
			if (!std::isdigit((*it)[i]))
				throw ;
		server.max_upload = std::atoi((*(it + 1)).c_str());
	}
	else if (*it == "root") {
		if (size != 2)
			throw ;
		server.root = *(it + 1);
	}
	else if (*it == "error_page") {
		if (size != 3)
			throw ;
		for (int i = 0; i < (*(it + 1)).size(); i++)
			if (!std::isdigit((*it)[i]))
				throw ;
		server.err_page.emplace(std::atoi((*(it + 1)).c_str()), *(it + 2));
	}
	else
		throw ;
}

static void	parse_param(std::vector<std::string> const &words, Location &location) {

	std::vector<std::string>::const_iterator	it = words.begin();
	std::vector<std::string>::const_iterator	ite = words.end();
	int											size = words.size();

	if (size <= 1)
		throw ;
	if (*it == "autoindex") {
		if (size != 2)
			throw ;
		if (*(it + 1) == "on")
			location.autoindex = 1;
		else if (*(it + 1) == "off")
			location.autoindex = 0;
		else
			throw ;
	}
	else if (*it == "index") {
		if (size != 2)
			throw ;
		location.index = *(it + 1);
	}
	else if (*it == "root") {
		if (size != 2)
			throw ;
		location.root = *(it + 1);
	}
	else if (*it == "allow_methods")
		for (it = words.begin() + 1; it != ite; ++it)
		{
			if (*it != "GET" && *it != "POST" && *it != "DELETE")
				throw ;
			location.methods.push_back(*it);
		}
	else
		throw ;
}

static void	parse_line(std::vector<std::string> const &words, std::vector<Server> &servers) {

	std::vector<std::string>::const_iterator	it = words.begin();
	static int	level = 0;

	if (*it == "server") {
		if (level != 0 || words.size() != 2 || *(it + 1) != "{")
			throw ;
		servers.emplace_back();
		level++;
	}
	else if (*it == "location") {
		if (level != 1 || words.size() != 3 || *(it + 2) != "{")
			throw ;
		servers.back().locations.emplace_back();
		servers.back().locations.back().path = *(it + 1);
		level++;
	}
	else if (*it == "}") {
		if (level < 1)
			throw ;
		level--;
	}
	else {
		if (level == 1)
			parse_param(words, servers.back());
		else if (level == 2)
			parse_param(words, servers.back().locations.back());
		else
			throw;
	}
}

void	parse_conf(std::string filename, std::vector<Server> &servers) {

	std::ifstream				file(filename.c_str());
	std::string					line;
	std::vector<std::string>	words;

	(void)servers;
	if (!file.is_open())
		throw ;
	while(std::getline(file, line))
	{
		std::string				buffer;
		std::string::iterator	its = line.begin();
		for (its = line.begin(); its != line.end(); ++its)
		{
			if (std::isspace(*its) || *its == ';' || *its == '#')
			{
				if (!buffer.empty())
					words.push_back(buffer);
				buffer.clear();
				if (*its == ';' || *its == '#')
					break;
			}
			else if (*its == '{' || *its == '}')
			{
				if (!buffer.empty())
					words.push_back(buffer);
				buffer.clear();
				std::string	temp(1, *its);
				words.push_back(temp);
			}
			else
				buffer.push_back(*its);
		}
	}
}

// server, listen, server_name, error_page, root, index, client_max_body_size //location, autoindex, allow_methods, cgi_path, cgi_extension, return

// int	parse_line(std::vector<std::string>	words, std::vector<Server> servers) {

// 	std::vector<std::string>::iterator	it = words.begin();
// 	static int	serv_ind = -1;
// 	static int	loc_ind = -1;
// 	static int	in_serv = 0;
// 	static int	in_loc = 0;

// 	for (it = words.begin(); it != words.end(); ++it)
// 	{
// 		if (*it == "server") {
// 			if (*(it + 1) != "{")
// 				throw ;
// 			in_serv++;
// 			serv_ind++;
// 			loc_ind = -1;
// 		}
// 		else if (*it == "location") {
// 			if (it + 1 == words.end() || *(it + 1) == "{"
// 				|| it + 2 == words.end() || *(it + 2) != "{")
// 				throw ;
// 			in_loc++;
// 			loc_ind++;
// 		}
// 		else if (*it == "}") {
// 			if (in_loc == 0 && in_serv == 0)
// 				throw ;
// 			if (in_loc == 1)
// 				in_loc--;
// 			else
// 				in_serv--;
// 		}
// 	}
// 	if (in_loc < 0 || in_loc > 1
// 		|| in_serv < 0 || in_serv > 1)
// 		throw ;
// 	return (1);
// }

// void	add_in_location(std::vector<std::string> &words, Location &location) {
	
// }


// void	add_in_server(std::vector<std::string> &words, Server &server) {
	
// }

// int	parse_line(std::vector<std::string>	&words, std::vector<Server> &servers) {

// 	std::vector<std::string>::iterator	it = words.begin();
// 	static int	level = 0;

// 	if (*it == "server") {
// 		if (words.size() > 2 || *(it + 1) != "{")
// 			throw ;
// 		level++;
// 		servers.emplace_back();
// 	}
// 	else if (*it == "location") {
// 		if (it + 1 == words.end() || *(it + 1) == "{"
// 			|| it + 2 == words.end() || *(it + 2) != "{"
// 			|| level != 1)
// 			throw ;
// 		level++;
// 		servers.back().locations.emplace_back();
// 	}
// 	else if (*it == "}") {
// 		if (level == 0)
// 			throw ;
// 		level--;
// 	}
// 	else {
// 		if (level == 2)
// 			add_in_location(words, servers.back().locations.back());
// 		else if (level == 1)
// 			add_in_server(words, servers.back());
// 	}
// 	if (level < 0 || level > 2)
// 		throw ;
// 	return (1);
// }
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_parsing.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:57:33 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/15 17:56:26 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"config.hpp"
#include<cstdlib>


// server, listen, server_name, error_page, root, index, client_max_body_size //location, autoindex, allow_methods, cgi_path, cgi_extension, return

int	parse_line(std::vector<std::string>	words, std::vector<Server> servers) {

	std::vector<std::string>::iterator	it = words.begin();
	static int	serv_ind = -1;
	static int	loc_ind = -1;
	static int	in_serv = 0;
	static int	in_loc = 0;

	for (it = words.begin(); it != words.end(); ++it)
	{
		if (*it == "server")
		{
			std::cout << "je suis un server" << std::endl;
			if (*(it + 1) != "{")
				throw ;
			in_serv++;
			serv_ind++;
			loc_ind = -1;
		}
		else if (*it == "location")
		{
			std::cout << "je suis un location" << std::endl;
			if (it + 1 == words.end() || *(it + 1) == "{"
				|| it + 2 == words.end() || *(it + 2) != "{")
				throw ;
			in_loc++;
			loc_ind++;
		}
		else if (*it == "}")
		{
			std::cout << "je suis une accolade" << std::endl;
			if (in_loc == 0 && in_serv == 0)
				throw ;
			if (in_loc == 1)
				in_loc--;
			else
				in_serv--;
		}
	}
	if (in_loc < 0 || in_loc > 1
		|| in_serv < 0 || in_serv > 1)
		throw ;
	return (1);
}

void	parse_conf(std::string filename, std::vector<Server> servers) {

	std::ifstream				file(filename.c_str());
	std::string					line;
	std::vector<std::string>	words;

	if (!file.is_open())
		throw ;
	while(std::getline(file, line))
	{
		words.clear();
		std::string	buffer;
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
		parse_line(words, servers);
	}
}
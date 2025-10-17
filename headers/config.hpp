/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:30:17 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/17 12:26:08 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>


// il faut encore ajoute les cgi mais on verra ca plus tard
struct Location {
	Location(void);
	~Location(void);

	int							autoindex;
	std::string					path;
	std::string					index;
	std::string					root;
	// std::string					redirect;
	std::string					cgi_path;
	std::string					cgi_extension;
	std::string					upload_dir;
	std::vector<std::string>	methods;
	std::vector<Location>		locations;
};

struct Server {
	Server(void);
	~Server(void);

	std::vector<int>			listen;
	std::string					server_name;
	std::string					root;
	unsigned long				max_upload;
	std::string					index_page;
	std::vector<Location>		locations;
	std::map<int, std::string>	err_page;
};

void	parse_conf(std::string filename, std::vector<Server> &servers);

std::ostream	&operator<<(std::ostream &o, Location const &loc);
std::ostream	&operator<<(std::ostream &o, Server const &serv);
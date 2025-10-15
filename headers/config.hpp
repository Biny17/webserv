/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:30:17 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/15 17:06:40 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

struct Location {
	Location(void);
	~Location(void);

	int							autoindex;
	std::string					path;
	std::string					index;
	std::string					root;
	std::string					redirect;
	std::string					upload_dir;
	std::vector<std::string>	methods;
};

struct Server {
	Server(void);
	~Server(void);

	int							listen;
	std::string					server_name;
	unsigned long				max_upload;
	std::string					index_page;
	std::vector<Location>		locations;
	std::map<int, std::string>	err_page;
};

std::ostream	&operator<<(std::ostream &o, Location const &loc);
std::ostream	&operator<<(std::ostream &o, Server const &serv);
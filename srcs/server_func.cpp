/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_func.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:08:12 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/16 14:03:49 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"config.hpp"

//Location

Location::Location(void) : autoindex(0), path(NULL), index(NULL), root(NULL), redirect(NULL), upload_dir(NULL) {
	return ;
}

Location::~Location(void) {
	return ;
}

std::ostream	&operator<<(std::ostream &o, Location const &loc) {
	if (!loc.path.empty())
	o << "path : " << loc.path << std::endl;
	if (!loc.root.empty())
		o << "root : " << loc.root << std::endl;
	o << "autoindex : " << loc.autoindex << std::endl;
	if (!loc.redirect.empty())
		o << "redirect : " << loc.path << std::endl;
	if (!loc.upload_dir.empty())
		o << "upload_dir : " << loc.path << std::endl;
	if (!loc.methods.empty())
	{
		o << "methods : ";
		for (size_t i = 0; i < loc.methods.size(); i++)
		{
			o << loc.methods[i] << " ";
		}
		o << std::endl;
	}
	return o;
}

//Server

Server::Server(void) : server_name(NULL), max_upload(0), index_page(NULL) {
	return ;
}

Server::~Server(void) {
	return ;
}

std::ostream	&operator<<(std::ostream &o, Server const &serv) {
	o << "listen : " << serv.listen[0] << std::endl;
	if (!serv.server_name.empty())
		o << "server_name : " << serv.server_name << std::endl;
	o << "max_upload : " << serv.max_upload << std::endl;
	if (!serv.index_page.empty())
		o << "index : " << serv.index_page << std::endl;
	if (!serv.locations.empty())
		o << "locations amount : " << serv.locations.size() << std::endl;
	if (!serv.err_page.empty())
		o << "error_page amount : " << serv.err_page.size() << std::endl;
	return o;
}
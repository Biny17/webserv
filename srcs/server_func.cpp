/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_func.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 11:08:12 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/17 15:12:57 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"config.hpp"

//Location

Location::Location(void) : autoindex(0) {
	return ;
}

Location::~Location(void) {
	return ;
}

std::ostream	&operator<<(std::ostream &o, Location const &loc) {
	
	std::cout << "++++++++++++++++++++++Location+++++++++++++++++++" << std::endl;
	if (!loc.path.empty())
		o << "path : " << loc.path << std::endl;
	if (!loc.root.empty())
		o << "root : " << loc.root << std::endl;
	o << "autoindex : " << loc.autoindex << std::endl;
	// if (!loc.redirect.empty())
	// 	o << "redirect : " << loc.path << std::endl;
	if (!loc.upload_dir.empty())
		o << "upload_dir : " << loc.path << std::endl;
	if (!loc.index.empty())
	{
		o << "index : ";
		for (std::vector<std::string>::const_iterator it = loc.index.begin(); it != loc.index.end(); ++it)
			o << *it << " ";
		o << std::endl;
	}
	if (!loc.cgi_path.empty())
		o << "cgi_path : " << loc.cgi_path << std::endl;
	if (!loc.cgi_extension.empty())
		o << "cgi_path : " << loc.cgi_extension << std::endl;
	if (!loc.methods.empty())
	{
		o << "methods : ";
		for (size_t i = 0; i < loc.methods.size(); i++)
		{
			o << loc.methods[i] << " ";
		}
		o << std::endl;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	return o;
}

//Server

Server::Server(void) : max_upload(0) {
	return ;
}

Server::~Server(void) {
	return ;
}

std::ostream	&operator<<(std::ostream &o, Server const &serv) {
	o << "---------------------------------server----------------------------------" << std::endl;
	if (!serv.listen.empty())
	{
		o << "listen : ";
		for (std::vector<int>::const_iterator it = serv.listen.begin(); it != serv.listen.end(); ++it)
			o << *it << " ";
		o << std::endl;
	}
	if (!serv.server_name.empty())
		o << "server_name : " << serv.server_name << std::endl;
	o << "max_upload : " << serv.max_upload << std::endl;
	if (!serv.index_page.empty())
	{
		o << "index : ";
		for (std::vector<std::string>::const_iterator it = serv.index_page.begin(); it != serv.index_page.end(); ++it)
			o << *it << " ";
		o << std::endl;
	}
	if (!serv.err_page.empty())
		o << "error_page amount : " << serv.err_page.size() << std::endl;
	if (!serv.locations.empty())
	{
		o << "locations amount : " << serv.locations.size() << std::endl;
		std::vector<Location>::const_iterator	it = serv.locations.begin();
		for (it = serv.locations.begin(); it != serv.locations.end(); ++it)
		{
			std::cout << *it << std::endl;
		}
	}
	o << "-------------------------------------------------------------------------" << std::endl;
	return o;
}
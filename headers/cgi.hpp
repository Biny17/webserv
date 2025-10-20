/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinton <tpinton@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 18:14:01 by tpinton           #+#    #+#             */
/*   Updated: 2025/10/20 18:15:28 by tpinton          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CGI_HPP
# define CGI_HPP

#include"webserv.hpp"

int							manage_cgi(std::string &filename, char **env);
std::vector<std::string>	list_file_extension(std::string const &directory, std::string const &ext);

#endif
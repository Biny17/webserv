#include "webserv.hpp"

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
		for (it = words.begin() + 1; it != words.end(); ++it)
			server.index_page.push_back(*it);
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
			location.autoindex = true;
		else if (*(it + 1) == "off")
			location.autoindex = false;
		else
			throw std::runtime_error(*it + " argument error");
	}
	else if (*it == "index") {
		for (it = words.begin() + 1; it != words.end(); ++it)
			location.index.push_back(*it);
	}
	else if (*it == "alias") {
		if (size != 2)
			throw std::runtime_error(*it + " argument error");
		location.alias = *(it + 1);
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
		for (it = words.begin() + 1; it != ite; ++it)
			location.cgi_extension.push_back(*it);
	}
	else if (*it == "return") {
		if (size != 3)
			throw std::runtime_error(*it + " argument error");
		for (size_t i = 0; i < (*(it + 1)).size(); i++)
			if (!std::isdigit((*(it + 1))[i]))
				throw std::runtime_error(*it + " argument error");
		std::pair<int, std::string>	err(std::atoi((*(it + 1)).c_str()), *(it + 2));
		location.redirect.insert(err);
	}
	else
		throw std::runtime_error("location unknown command : " + *it);
}

static void	delete_loc_doublon(std::vector<Location> &locations, std::string const &path) {

	std::vector<Location>::iterator	it = locations.begin();
	if (locations.empty())
		return ;
	while (it != locations.end())
	{
		if ((*it).path == path)
			it = locations.erase(it);
		else
			++it;
	}
	return ;
}

//point de passage de toute les ligne, sert a savoir si on entre dans un bloc ou si on quite un bloc, ou alors si on a un argument de bloc
static void	parse_line(std::vector<std::string> const &words, std::vector<Server> &servers, int &level) {

	std::vector<std::string>::const_iterator	it = words.begin();

	if (*it == "server") {
		if (level != 0 || words.size() != 2 || *(it + 1) != "{")
			throw std::runtime_error(*it + " bad line");
		construct_back<Server>(servers);
		level++;
	}
	else if (*it == "location") {
		if (level != 1 || words.size() != 3 || *(it + 2) != "{")
			throw std::runtime_error(*it + " bad line");
		delete_loc_doublon(servers.back().locations, *(it + 1));
		construct_back<Location>(servers.back().locations);
		servers.back().locations.back().path = *(it + 1);
		add_leading_slash(trim_trailing_slash(servers.back().locations.back().path));
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

//check every location of a server bloc and test access to it and access to every index file in it
void	check_location(Server const &server) {

		std::vector<Location>::const_iterator	it;
		std::vector<Location>::const_iterator	ite = server.locations.end();

		//check les path des location
		for (it = server.locations.begin(); it != ite; ++it)
		{
			if ((*it).alias.empty())
			{
				if (access((server.root + (*it).path).c_str(), F_OK))
					throw std::runtime_error("cant access " + (*it).path);
			}
			else
			{
				if (access(((*it).alias + (*it).path).c_str(), F_OK))
					throw std::runtime_error("cant access " + (*it).path);
			}

			//check les index file du bloc location
			std::vector<std::string>::const_iterator itl;
			std::vector<std::string>::const_iterator itle = (*it).index.end();
			for (itl = (*it).index.begin(); itl != itle; ++itl)
			{
				if ((*it).alias.empty())
				{
					if (access((server.root + (*it).path + *itl).c_str(), F_OK))
						throw std::runtime_error("cant access " + *itl);
				}
				else
				{
					if (access(((*it).alias + (*it).path + *itl).c_str(), F_OK))
						throw std::runtime_error("cant access " + *itl);
				}
			}
		}
}

//check for access error in different path or file in server configuration
void	access_check(std::vector<Server> const &servers) {

	std::vector<Server>::const_iterator	it;
	std::vector<Server>::const_iterator	ite = servers.end();

	for (it = servers.begin(); it != ite; ++it)
	{
		check_location(*it);

		//check for access error in server index
		std::vector<std::string>::const_iterator	ita;
		std::vector<std::string>::const_iterator	itae = (*it).index_page.end();
		for (ita = (*it).index_page.begin(); ita != itae; ++ita)
		{
			if (access(((*it).root + *ita).c_str(), F_OK))
				throw std::runtime_error("cant access " + *ita);
		}

		//check for access error in error file
		std::map<int, std::string>::const_iterator	itl;
		std::map<int, std::string>::const_iterator	itle = (*it).err_page.end();
		for (itl = (*it).err_page.begin(); itl != itle; ++itl)
		{
			if (access(((*it).root + itl->second).c_str(), F_OK))
				throw std::runtime_error("cant access " + itl->second);
		}
	}
}

void	fill_extension(Server &server) {

	std::vector<Location>::iterator	it;
	std::vector<Location>::iterator	ite = server.locations.end();

	for (it = server.locations.begin(); it != ite; ++it) {
		(*it).extension[".html"] = "text/html";
		(*it).extension[".css"] = "text/css";
		(*it).extension[".js"] = "text/javascript";
		if (!(*it).cgi_extension.empty())
		{
			std::vector<std::string>::iterator	itl;
			std::vector<std::string>::iterator	itle = (*it).cgi_extension.end();

			for (itl = (*it).cgi_extension.begin(); itl != itle; ++itl)
			{
				if (*itl != ".py" && *itl != ".sh")
					throw std::runtime_error("invalid cgi_extension or cgi not managed");
				if (*itl == ".py")
					(*it).extension[*itl] = "text/python";
				if (*itl == ".sh")
					(*it).extension[*itl] = "text/shell";
			}
		}
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
	std::vector<Server>::iterator	it;
	std::vector<Server>::iterator	ite = servers.end();
	for (it = servers.begin(); it != ite; ++it)
		fill_extension(*it);
	// access_check(servers);	//!!!!!! pas encore vraiment test !!!!!!
}

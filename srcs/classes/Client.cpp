#include "Client.hpp"
#include <algorithm>

Client::Client(Server &s)
	:server(s), response(*this), parser(this->request, this->response)
{
	this->fd = -1;

	this->isCGI = false;
	this->CGIpid = -1;
	this->cat = "mouli1";
	this->changedCat = false;
	this->timeout.Stop();
}

Client::Client(const Client& other)
	: server(other.server), fd(other.fd), response(*this), parser(this->request, this->response)
{
	this->isCGI = other.isCGI;
	this->CGIpid = other.CGIpid;
	this->referringFD = other.referringFD;
	this->cat = other.cat;
	this->changedCat = other.changedCat;
	this->timeout = other.timeout;
}

Client::~Client(void)
{
	if (this->fd >= 0)
		close(this->fd);
}

// Define the client as a CGI and add it to epoll
void	Client::setCGI(int referringFD)
{
	this->isCGI = true;
	this->referringFD = referringFD;

	struct epoll_event cli_event;
	cli_event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	cli_event.data.fd = this->fd;
	this->epollStatus = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP;
	if (epoll_ctl(this->server.epfd, EPOLL_CTL_ADD, this->fd, &cli_event) == -1)
	{
		std::cout << "Couldn't add the cgi to epoll" << std::endl;
		this->server.removeClient(this->fd);
	}
	this->timeout.Start();
}

size_t n_prefix_match(std::string &target, std::string &location)
{
	size_t n = 0;
	while (target[n] == location[n]
			&& n < target.length()
			&& n < location.length())
		n++;
	if (n == location.length())
		return n;
	return 0;
}

void Client::checkLocation()
{
	loc = &server.locations[0];
	size_t biggest = 1;

	size_t i = 0;
	while(i < server.locations.size()) {
		size_t tmp = n_prefix_match(request.path, (server.locations[i]).path);
		std::cout << request.path << " " << (server.locations[i]).path << "  " << tmp << std::endl;
		if (tmp > biggest) {
			biggest = tmp;
			loc = &server.locations[i];
		}
		i++;
	}
	request.loc_index = i;
	BuildPath(*loc);
	if (std::find(loc->methods.begin(), loc->methods.end(), request.method) == loc->methods.end())
		return Error403(*loc);
	parser.state = BODY;
	if (request.content_len > server.max_upload) {
		response.code = 413;
		parser.state = ERROR;
		return ;
	}
	parser.max_body_size = server.max_upload;
}

void Client::Error403(Location& loc)
{
	std::string allow("Allow: ");
	for (std::vector<std::string>::iterator it = loc.methods.begin(); it != loc.methods.end(); it++) {
		if (it != loc.methods.begin())
			allow += ", ";
		allow += *it;
	}
	response.code = 403;
	response.headers.push_back(allow);
	parser.state = ERROR;
}

void  Client::BuildPath(Location& loc)
{
	if (loc.root.empty())
		request.local_path = server.root + request.path;
	else
		request.local_path = loc.root + request.path;
}

static bool is_cgi(Location& loc, std::string& target)
{
	for (int i = 0; i < loc.cgi_extension.size(); i++)
	{
		if (loc.cgi_extension[i] == get_extension(target))
			return (true);
	}
	return (false);
}

void Client::PostFile()
{
	size_t boundary_index = request.headers.find("Content-Type")->second.find("boundary=");
	std::string boundary = request.headers.find("Content-Type")->second.substr(boundary_index + 9);

	if ()
}

void Client::RequestHandler()
{
	std::cout << "location matched: " << loc->path << std::endl << std::endl;

	// ajouter test cgi
	if (is_cgi(*loc, request.path))
		launch_cgi(request.local_path, server, *this);
	else if (request.method == "GET")
		get_static_file(server, *this, request, response);
	else if (request.method == "POST")
		PostFile();
}

void	Client::switchCat(void)
{
	this->changedCat = true;
	if (this->request.headers.find("Cookie") != this->request.headers.end())
		this->cat = this->request.headers["Cookie"].substr(7, 13);
	this->cat = this->cat == "mouli1" ? "mouli2" : "mouli1";
}

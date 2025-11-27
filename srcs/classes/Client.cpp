#include "Client.hpp"
#include <algorithm>
#include "../../headers/webserv.hpp"

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

bool	Client::Error(int error_code)
{
	response.code = error_code;
	parser.state = ERROR;
	return false;
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
	request.loc_index = 0;
	while(i < server.locations.size()) {
		size_t tmp = n_prefix_match(request.path, (server.locations[i]).path);
		// std::cout << request.path << " " << (server.locations[i]).path << "  " << tmp << std::endl;
		if (tmp > biggest) {
			biggest = tmp;
			loc = &server.locations[i];
			request.loc_index = i;
		}
		i++;
	}
	BuildPath(*loc);
	if (std::find(loc->methods.begin(), loc->methods.end(), request.method) == loc->methods.end())
	{
		if (loc->methods.size() == 0)
			return (void)Error(403);
		else
			return Error405(*loc);
	}
	// std::cout << "content_len: " << request.content_len << std::endl;
	// std::cout << "max_upload: " << server.max_upload << std::endl;
	if (request.content_len > static_cast<int>(server.max_upload)) {
		response.code = 413;
		parser.state = ERROR;
		return ;
	}
	if (request.headers.find("Host")->second != server.server_name)
	{
		response.code = 400;
		parser.state = ERROR;
		return ;
	}
	parser.state = BODY;
}

void Client::Error405(Location& loc)
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
	for (size_t i = 0; i < loc.cgi_extension.size(); i++)
	{
		if (loc.cgi_extension[i] == get_extension(target))
			return (true);
	}
	return (false);
}

bool Client::MultipartFormData()
{
    std::string& b = request.body;
	std::map<std::string, std::string>::iterator ct = request.headers.find("Content-Type");
	std::string bnd = ct->second.substr(ct->second.find("boundary=") + 9);
	bnd = bnd.substr(0, bnd.find_first_of("; "));
	size_t cur = 0;

	while (true)
	{
		if (!extract_boundary(b, cur, bnd))
			return Error(400);
		bool crlf = b.compare(cur, 2, "\r\n") == 0;
		bool end = b.compare(cur, 2, "--") == 0;
		cur += 2;
		if (end)
			break;
		if (!crlf)
			return Error(400);
		size_t header_end;
		if (!validate_headers(b, cur, header_end))
			return Error(400);
		std::string filename = extract_filename(b, cur);
		if (!valid_filename(filename))
			return Error(400);
		if (access(filename.c_str(), F_OK) == 0)
			return Error(409);
		cur = header_end + 4;
		// print_hex_string(b.substr(cur));
		size_t data_end = b.find("\r\n", cur);
		if (data_end == std::string::npos)
			return Error(400);
		if (!write_file(filename, b, cur, data_end))
			return Error(500);
		cur = data_end + 2;
	}
	return true;
}

void Client::PostFile()
{
	std::string content_type = request.headers.find("Content-Type")->second;
	if (content_type.find("multipart/form-data") != std::string::npos)
	{
		if (!MultipartFormData())
			return;
		response.code = 201;
		parser.state = RESPONSE;
	}
	else
	{
		response.code = 204;
		parser.state = RESPONSE;
	}
}

void Client::RequestHandler()
{
	if (is_cgi(*loc, request.path))
		launch_cgi(request.local_path, server, *this);
	else if (request.method == "GET")
		get_static_file(server, request, response);
	else if (request.method == "POST")
		PostFile();
	else if (request.method == "DELETE")
		build_delete_response(server, request, response);
	parser.state = RESPONSE;
}

void	Client::switchCat(void)
{
	this->changedCat = true;
	if (this->request.headers.find("Cookie") != this->request.headers.end())
		this->cat = this->request.headers["Cookie"].substr(7, 13);
	this->cat = this->cat == "mouli1" ? "mouli2" : "mouli1";
}

#include "webserv.hpp"

Response::Response(Client& Client): client(Client)
{
	this->code = 200;
	this->body = "";
	this->outBuffer = "";
	this->content_type = "";
	return ;
}

Response::~Response(void) {}

const char*	Response::Reason(void)
{
	switch (this->code)
	{
		// 1xx
		case 100: return "Continue";
		case 101: return "Switching Protocols";

		// 2xx
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 204: return "No Content";

		// 3xx
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";

		// 4xx
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 409: return "Conflict";
		case 411: return "Length Required";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 418: return "I'm a teapot";
		case 429: return "Too Many Requests";

		// 5xx
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";

		default:  return "Unknown";
	}
}

std::string	Response::Date(void)
{
	char		buf[64];
	std::time_t	now = std::time(0);
	struct tm	gmt;

	gmt = *std::gmtime(&now);
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gmt);
	return (std::string(buf));
}

std::string	Response::GetConnection(void)
{
	if (this->client.request.headers.find("Connection") != this->client.request.headers.end())
		return (client.request.headers["Connection"]);
	return ("close");
}

std::string	Response::Header(void)
{
	std::stringstream	header;

	header << "HTTP/1.1 " << this->code << " " << this->Reason() << "\r\n";
	header << "Date: " << this->Date() << "\r\n";
	header << "Server: webserv\r\n";
	header << this->Cat();
	if (this->body != "")
	{
		header << "Content-Type: " << this->content_type << "\r\n";
		header << "Content-Length: " << this->body.size() << "\r\n";
	}
	header << "Connection: " << this->GetConnection() << "\r\n";
	header << "\r\n";

	return (header.str());
}

void	Response::ReplaceCat(void)
{
	size_t	pos;
	std::string replace = this->client.cat == "mouli1" ? "class=\"mouli2\"" : "class=\"mouli1\"";
	pos = this->body.find(replace);
	while (pos != this->body.npos)
	{
		this->body.replace(pos, 6, replace == "class=\"mouli1\"" ? "class=\"mouli2\"" : "class=\"mouli1\"");
		pos += 6;
		pos = this->body.find(replace);
	}
}

std::string	Response::Cat()
{
	if (this->client.changedCat == false)
		return ("");
	this->client.changedCat = false;
	return ("Set-Cookie: cookie=" + this->client.cat + "\r\n");
}

std::string	Response::FindPage(void)
{
	if (this->code <= 0)
		return ("");

	std::map<int, std::string>::iterator search = this->client.server.err_page.find(this->code);
	if (search == this->client.server.err_page.end())
		return ("");

	return (search->second);
}

std::string Response::ReadFile(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file)
	{
		std::cout << "An error occured while opening the request response" << std::endl;
		return ("");
	}

	this->content_type = "text/html; charset=utf-8";

	return std::string(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);
}

// Build the response of the server
void	Response::Build(void)
{
	std::string page = this->FindPage();
	if (page != "")
		this->body = this->ReadFile(page);
	if (this->content_type == "text/html; charset=utf-8")
		this->ReplaceCat();
	if (page == "" && this->body == "") // For tests only
		this->body = this->ReadFile("www/html/index.html");

	this->outBuffer = this->Header() + this->body;
}

void	Response::BuildCGI(void)
{
	std::ifstream file("www/html/cgi.html");
	if (file)
	{
		std::string cgi_body(
			(std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>()
		);

		size_t	pos = cgi_body.find("</main>");
		if (pos != cgi_body.npos)
		{
			this->body.insert(0, "\t");
			this->body += "\t";
			this->body = cgi_body.insert(pos, this->body);
		}

		this->content_type = "text/html; charset=utf-8";
	}

	this->Build();
}

// Send the server's response to the client
void	Response::Send(void)
{
	ssize_t	bytes = send(this->client.fd, this->outBuffer.c_str(), this->outBuffer.size(), 0);

	if (bytes < 0)
		return ;

	std::cout << std::endl << COLOR_GREEN << "------- RESPONSE -------" << std::endl;
	std::cout << outBuffer.substr(0, bytes) << COLOR_NC;

	if (bytes > 0)
		this->outBuffer.erase(0, bytes);

	if (this->outBuffer.empty())
	{
		std::cout << std::endl;
		if (this->GetConnection() == "close")
		{
			this->client.server.removeClient(this->client.fd);
			return ;
		}

		if (this->client.epollStatus & EPOLLOUT)
			set_epoll_event(this->client.server.epfd, this->client, EPOLLIN);

		return ;
	}

	if (this->client.epollStatus & EPOLLIN)
		set_epoll_event(this->client.server.epfd, this->client, EPOLLOUT);
}

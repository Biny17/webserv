#include "webserv.hpp"

Parser::Parser(Request& request, Response& response)
	: max_body_size(16384), skip_leading_ws(true),
	req(request), err(response), ok(true), state(INIT) {}

Parser::Parser(const Parser& Parser): req(Parser.req), err(Parser.err)
{
	*this = Parser;
}

Parser&	Parser::operator=(const Parser& Parser)
{
	if (this == &Parser)
		return (*this);
	this->max_body_size = Parser.max_body_size;
	this->state = Parser.state;
	this->skip_leading_ws = Parser.skip_leading_ws;
	this->req = Parser.req;
	this->err = Parser.err;
	this->ok = Parser.ok;
	return (*this);
}

void Parser::Reset()
{
	state = INIT;
	ok = true;
	skip_leading_ws = true;
	cur_key.clear();
	cur_value.clear();
	req.content_len = 0;
	req.headers.clear();
	req.method.clear();
	req.query.clear();
	req.version.clear();
	req.path.clear();
}

void Parser::Error(std::string msg, int error_code)
{
	state = ERROR;
	ok = false;
	if (msg == "")
		err.body = "malformed request syntax";
	else
		err.body = msg;
	err.code = error_code;
}

void Parser::Method(const std::string& buff, size_t& i)
{
	if (parse_token(buff, req.method, i, 15) > 6) {
		Error("Unsupported method", 501);
		return ;
	}

	if (i == buff.length())
		return;

	if (buff[i] != ' ') {
		Error("", 400);
		return;
	}
	if (!valid_method(req.method)) {
		Error("Unsupported method", 501);
		return;
	}
	state = PATH;
	i++;
}

void Parser::Path(const std::string& buff, size_t& i)
{
	size_t start = i;
	while (i < buff.length() && is_abspath(buff[i]))
		i++;
	if ((req.path.length() + i - start) > URI_MAX) {
		Error("Uri is too long", 414);
		return;
	}
	req.path += buff.substr(start, i - start);
	if (i == buff.length())
		return;
	if ((buff[i] == ' ' || buff[i] == '?') && valid_path(req.path)) {
		state = (buff[i] == ' ') ? VERSION : QUERY;
		i++;
	}
	else
		Error("", 400);
}

void Parser::Query(const std::string &buff, size_t& i)
{
	size_t start = i;
	while (i < buff.length() && is_query(buff[i])
		&& (req.path.length() + req.query.length() + i - start) < URI_MAX+1)
		i++;
	req.query += buff.substr(start, i - start);
	if (req.query.length()+req.path.length() > URI_MAX)
		return Error("Uri is too long", 414);
	if (i == buff.length())
		return;
	if (buff[i] == ' ') {
		state = VERSION;
		i++;
	}
	else
		Error("", 400);
}

void Parser::Version(const std::string &buff, size_t& i)
{
	size_t start = i;
	while (i < buff.length()
		&& (std::isalnum(buff[i]) || buff[i] == '/' || buff[i] == '.' )
		&& (i-start < 10))
		i++;
	req.version += buff.substr(start, i-start);
	if (req.version != "HTTP/1.1" && req.version != "HTTP/1.0")
		return Error("Unsupported HTTP version", 505);
	if (i+1 < buff.length())
	{
		if (buff[i] == '\r' && buff[i+1] == '\n') {
			state = HEAD_KEY;
			i = i+2;
		}
		else
			Error("", 400);
	}
}

void Parser::HeadKey(const std::string &buff, size_t& i)
{
	if (i+1 < buff.length())
	{
		if (buff[i] == '\r' && buff[i+1] == '\n') {
			state = COMPLETE;
			i += 2;
			AfterHeadersCheck();
			return;
		}
	}
	else
		return;
	size_t start = i;
	while (i < buff.length() && is_token(buff[i]))
		i++;
	if ((i-start + cur_key.length()) > HEADER_MAX)
		return Error("Request Header Fields Too Large", 431);
	cur_key += buff.substr(start, i - start);
	if (i == buff.length())
		return;
	if (buff[i] != ':') {
		Error("", 400);
		return;
	}
	state = HEAD_VAL;
	skip_leading_ws = true;
	i++;
}

void Parser::HeadValue(const std::string &buff, size_t& i)
{
	size_t start = i;

	if (skip_leading_ws) {
		while (i < buff.length() && (buff[i] == ' ' || buff[i] == '\t'))
			i++;
		start = i;
		if (i < buff.length())
			skip_leading_ws = false;
	}

	size_t crfl = buff.find("\r\n", i);
	if (crfl == std::string::npos) {
		if (cur_value.length() + (i - buff.length()) > HEADER_MAX)
			return Error("Request Header Fields Too Large", 431);
		cur_value += buff.substr(start, i - buff.length());
		i = buff.length();
		return;
	}
	else {
		cur_value += buff.substr(start, crfl - start);
		cur_value = cur_value.substr(0, cur_value.find_last_not_of(" \t") + 1);
		req.headers[cur_key] = cur_value;
		if (req.headers.size() > 100)
			return Error("Too many headers", 431);
		cur_key.clear();
		cur_value.clear();
		state = HEAD_KEY;
		i = crfl + 2;
	}
}

void Parser::AfterHeadersCheck()
{
	if (req.headers.find("Host") == req.headers.end())
		return Error("Host header is required", 400);
	if (req.method == "GET")
	{
		if (req.headers.find("Content-Length") != req.headers.end() ||
			req.headers.find("Transfer-Encoding") != req.headers.end())
			return Error("GET request must not have a body", 400);
	}
	else if (req.method == "POST") {
		PostCheck();
	}
}

void Parser::PostCheck()
{
	std::map<std::string,std::string>::iterator te = req.headers.find("Transfer-Encoding");
	if (te != req.headers.end())
	{
		if (te->second != "chunked")
			return Error("Unsupported Transfer-Encoding", 501);
		else {
			state = CHUNK_SIZE;
			return;
		}
	}
	std::map<std::string,std::string>::iterator ce = req.headers.find("Content-Length");
	if (ce != req.headers.end())
	{
		std::stringstream ss(ce->second);
		ss >> req.content_len;
		if (ss.fail() || !ss.eof() || ss.bad() || req.content_len < 0)
			return Error("Invalid Content-Length", 400);
		if (req.content_len > max_body_size)
			return Error("Payload Too Large", 413);
	}
	else
		return Error("POST request must have a body", 400);
}

size_t Parser::FillReq(const std::string& read_buff)
{
	buff += read_buff;
	size_t i = 0;
	if (state == ERROR)
		return i;
	if (state == INIT)
		state = METHOD;
	if (state == METHOD && i < buff.length())
		Method(buff, i);
	if (state == PATH && i < buff.length())
		Path(buff, i);
	if (state == QUERY && i < buff.length())
		Query(buff, i);
	if (state == VERSION && i < buff.length())
		Version(buff, i);
	while ((state == HEAD_KEY || state == HEAD_VAL) && i < buff.length())
	{
		if (state == HEAD_KEY)
			HeadKey(buff, i);
		if (state == HEAD_VAL && i < buff.length())
			HeadValue(buff, i);
	}
	buff = read_buff.substr(i);
	return i;
}
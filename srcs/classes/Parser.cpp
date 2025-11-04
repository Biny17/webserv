#include "webserv.hpp"

Parser::Parser(Request& request, Response& response)
	: max_body_size(16384), skip_leading_ws(true), f(&Parser::DefaultBody),
	req(request), err(response), ok(true), state(INIT) {}

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
	req.loc_index = 0;
}

void Parser::Error(std::string msg, int error_code)
{
	state = ERROR;
	ok = false;
	if (msg == "")
		err.body = "Bad Request";
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
			state = BODY;
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
		cur_value = cur_value.substr(0, cur_value.find_first_of(';'));
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
			return Error("GET request shoudn't have a body", 400);
	}
	else if (req.method == "POST") {
		PostCheck();
	}
}

void Parser::PostCheck()
{
	std::map<std::string,std::string>::iterator ct = req.headers.find("Content-Type");
	if (ct == req.headers.end() || ct->second.find("multipart/form-data") == std::string::npos
			|| ct->second.find("boundary=") == std::string::npos)
		return Error("POST method require Content-Type: multipart/form-data with boundary", 400);

	boundary = ct->second.substr(ct->second.find("boundary=") + 9);

	std::map<std::string,std::string>::iterator te = req.headers.find("Transfer-Encoding");
	if (te != req.headers.end())
	{
		if (te->second != "chunked")
			return Error("Unsupported Transfer-Encoding", 501);
		else
			f = &Parser::TransferEncoding;
		return;
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
	else if (te == req.headers.end())
		return Error("POST request must have a body", 400);
}

void Parser::StateParsing(const std::string& read_buff, size_t& i)
{
	if (state == ERROR)
		return;
	if (state == INIT)
		state = METHOD;
	if (state == METHOD && i < p_buff.length())
		Method(p_buff, i);
	if (state == PATH && i < p_buff.length())
		Path(p_buff, i);
	if (state == QUERY && i < p_buff.length())
		Query(p_buff, i);
	if (state == VERSION && i < p_buff.length())
		Version(p_buff, i);
	while ((state == HEAD_KEY || state == HEAD_VAL) && i < p_buff.length())
	{
		if (state == HEAD_KEY)
			HeadKey(p_buff, i);
		if (state == HEAD_VAL && i < p_buff.length())
			HeadValue(p_buff, i);
	}
	if (state != ERROR && state != BODY)
		p_buff = read_buff.substr(i);
	return;
}


size_t Parser::FillReq(const std::string& read_buff)
{
	size_t i = 0;
	size_t tmp;
	if (p_buff.size() != 0) {
		tmp = p_buff.length();
		p_buff += read_buff;
		StateParsing(p_buff, i);
		i += -tmp;
		return i;
	}
	else {
		StateParsing(read_buff, i);
		return i;
	}
}

void Parser::DefaultBody(const std::string& buff, size_t i)
{
	size_t to_read = std::min(req.content_len - req.body.length(), buff.length() - i);
	req.body += buff.substr(i, to_read);
	if (req.body.length() == static_cast<size_t>(req.content_len))
		state = HANDLE;
}

void Parser::TransferEncoding(const std::string &buff, size_t i)
{
    p_buff += buff;

    while (i < p_buff.length()) {
        if (req.content_len == 0) {
            size_t crlf = p_buff.find("\r\n", i);
            if (crlf == std::string::npos) {
                return;
            }

            std::string chunk_size_str = p_buff.substr(i, crlf - i);
            std::stringstream ss(chunk_size_str);
            ss >> std::hex >> req.content_len;

            if (ss.fail() || req.content_len < 0) {
                Error("Invalid chunk size", 400);
                return;
            }
            i = crlf + 2;
            if (req.content_len == 0) {
                size_t final_crlf = p_buff.find("\r\n", i);
                if (final_crlf == i) {
                    state = HANDLE;
                } else {
                    Error("Invalid chunked encoding termination", 400);
                }
                return;
            }
        }
        size_t to_read = std::min(static_cast<size_t>(req.content_len), p_buff.length() - i);
        req.body += p_buff.substr(i, to_read);
        req.content_len -= to_read;
        i += to_read;
        if (req.content_len == 0) {
            if (i + 1 < p_buff.length() && p_buff[i] == '\r' && p_buff[i + 1] == '\n') {
                i += 2;
            } else {
                Error("Invalid chunk data termination", 400);
                return;
            }
        }
    }
    p_buff = p_buff.substr(i);
}
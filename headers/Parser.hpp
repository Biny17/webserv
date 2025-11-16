#ifndef PARSER_HPP
# define PARSER_HPP

# include "webserv.hpp"

enum p_state {
	METHOD,
	PATH,
	QUERY,
	VERSION,
	HEAD_KEY,
	HEAD_VAL,
	CHECK,
	BODY,
	HANDLE,
	RESPONSE,
	ERROR
};

class Parser {
	private:
		static const unsigned int URI_MAX = 8192;
		static const unsigned int HEADER_MAX = 4096;

		std::string cur_key;
		std::string cur_value;
		std::string p_buff;
		bool skip_leading_ws;

		void Error(int error_code);
		void Method(const std::string& buff, size_t& i);
		void Path(const std::string& buff, size_t& i);
		void Query(const std::string &buff, size_t& i);
		void Version(const std::string &buff, size_t& i);
		void HeadKey(const std::string &buff, size_t& i);
		void HeadValue(const std::string &buff, size_t& i);
		void AfterHeadersCheck();
		void PostCheck();
		void DefaultBody(const std::string &buff, size_t i);
		void TransferEncoding(const std::string &buff, size_t i);
		void StateParsing(const std::string& read_buff, size_t& i);

	public:
		void (Parser::*f)(const std::string &buff, size_t i);
		Request& req;
		Response& err;
		bool ok;
		p_state state;
		size_t FillReq(const std::string& buff);
		Parser(Request& request, Response& response);
		void Reset();
		void Print();
};


bool is_req(unsigned char c);
bool is_token(unsigned char c);
int parse_token(const std::string& src, std::string& dest, size_t &start, size_t limit);
bool is_pchar(unsigned char c);
bool valid_method(const std::string& method);
bool is_abspath(unsigned char c);
bool valid_path(const std::string& path);
bool is_query(unsigned char c);

#endif // PARSER_HPP

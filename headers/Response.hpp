#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include <vector>

class Client;

class Response {

	private:
		const char*	Reason(void);
		std::string	Date(void);
		std::string	GetConnection(void);
		std::string	FindPage(void);
		void		AppendError(std::string &file);
		std::string	ReadFile(const std::string &path);
		std::string	Header(void);
		std::string	Cat(void);
		void		ReplaceCat(void);

	public:
		Response(Client& client);
		~Response(void);

		Client&	client;

		int			code;
		std::string	content_type;
		std::string	body;
		std::string	outBuffer;
		std::vector<std::string> headers;

		void	Build(void);
		void	BuildCGI(void);
		void	Send(void);

};

#endif // RESPONSE_HPP

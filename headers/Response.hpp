#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"

class Client;

class Response {

	private:
		std::stringstream	header;

		const char*	Reason(void);
		std::string	Date(void);
		std::string	GetConnection(void);
		std::string	FindPage(void);
		std::string	ReadFile(const std::string &path);
		void		CreateHeader(void);

	public:
		Response(Client& client);
		Response(const Response& Response);
		~Response(void);

		Response&	operator=(const Response& Response);

		Client&	client;

		int			code;
		std::string	content_type;
		std::string	body;
		std::string	outBuffer;

		void	Build(void);
		void	Send(void);

};

#endif // RESPONSE_HPP

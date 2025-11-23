#include "webserv.hpp"

void	decode(std::string& str)
{
	size_t pos = 0;
	while ((pos = str.find('%', pos)) != std::string::npos)
	{
		if (pos + 2 >= str.size())
			return ;
		unsigned char c1 = static_cast<unsigned char>(str[pos + 1]);
		unsigned char c2 = static_cast<unsigned char>(str[pos + 2]);
		if (!std::isxdigit(c1) || !std::isxdigit(c2))
		{
			++pos;
			continue;
		}
		int hi = std::isdigit(c1) ? (c1 - '0') : (std::toupper(c1) - 'A' + 10);
		int lo = std::isdigit(c2) ? (c2 - '0') : (std::toupper(c2) - 'A' + 10);
		char decoded = static_cast<char>((hi << 4) | lo);
		str.replace(pos, 3, 1, decoded);
		++pos;
	}
	return ;
}

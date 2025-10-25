CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -ggdb -std=c++98

HEADERS		= ./headers/
SRC_DIR		= ./srcs/
OBJ_DIR		= ./objs/

CXXFLAGS += -I$(HEADERS)

SRCS		=	main.cpp						\
				cgi/get_cgi.cpp					\
				cgi/handle_cgi.cpp				\
				cgi/pipex_cgi.cpp				\
				classes/Client.cpp				\
				classes/Location.cpp			\
				classes/Server.cpp				\
				conf/conf_parsing.cpp			\
				network/epoll_ctl.cpp			\
				network/event_loop.cpp			\
				network/handle_clients.cpp		\
				network/launch_server.cpp		\
				parse_http/parse_utils.cpp		\
				parse_http/Parser.cpp			\
				parse_http/TestUtils.cpp		\
				utils/exit_signal.cpp			\
				utils/check_method.cpp

OBJS		= $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRCS))
DEPS		= $(OBJS:.o=.d)

NAME		= webserv

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re

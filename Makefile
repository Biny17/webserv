CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -ggdb

HEADERS		= ./headers/
SRC_DIR		= ./srcs/
OBJ_DIR		= ./objs/

CXXFLAGS += -I$(HEADERS)

SRCS		=	main.cpp							\
				cgi/get_cgi.cpp						\
				cgi/handle_cgi.cpp					\
				cgi/pipex_cgi.cpp					\
				classes/Client.cpp					\
				classes/Location.cpp				\
				classes/Mime.cpp					\
				classes/Parser.cpp					\
				classes/Response.cpp				\
				classes/Server.cpp					\
				classes/Timeout.cpp					\
				conf/conf_parsing.cpp				\
				network/epoll_ctl.cpp				\
				network/event_loop.cpp				\
				network/handle_clients.cpp			\
				network/launch_server.cpp			\
				utils/parse_utils.cpp				\
				utils/test_utils.cpp				\
				utils/exit_signal.cpp				\
				utils/autoindex.cpp					\
				utils/post_utils.cpp				\
				utils/path_utils.cpp				\
				utils/percent_encoding.cpp			\
				handle_request/get_response.cpp		\
				handle_request/delete_response.cpp

OBJS		= $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRCS))
DEPS		= $(OBJS:.o=.d)

NAME		= webserv

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@mkdir -p www/data

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

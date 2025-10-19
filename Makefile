CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -ggdb

HEADERS		= ./headers/
SRC_DIR		= ./srcs/
OBJ_DIR		= ./objs/

CXXFLAGS += -I$(HEADERS)

SRCS		=	main.cpp					\
				classes/Client.cpp			\
				classes/Server.cpp			\
				classes/Location.cpp		\
				conf/conf_parsing.cpp		\
				network/event_loop.cpp		\
				network/handle_clients.cpp	\
				network/launch_server.cpp	\
				utils/exit_signal.cpp

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

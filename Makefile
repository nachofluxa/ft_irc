NAME			:=	ircserv

SRC				:=	src/
HEA				:=	headers/
BUI				:=	build/

SRCS			:=	main.cpp \
					Server/AServer.cpp \
					Server/AServerClient.cpp \
					Client/AClient.cpp \
					Socket/Socket.cpp \
					IRC/IRCClient.cpp \
					IRC/IRCCommand.cpp \
					IRC/IRCServer.cpp \
					IRC/Channel.cpp

OBJS			:=  $(addprefix $(BUI), $(SRCS:.cpp=.o))
DEPS			:=  $(addprefix $(BUI), $(SRCS:.cpp=.d))

CXX				:=	c++
CXXFLAGS		:=	-Wall -Wextra -Werror -std=c++98 -MMD

RESET			:=	\033[0m
RED				:=	\033[0;91m
GREEN			:=	\033[0;92m
YELLOW			:=	\033[0;93m
BOLD			:=	\033[1m

all				:	$(NAME)

$(BUI)%.o		:	$(SRC)%.cpp
					@ mkdir -p $(dir $@)
					@ $(CXX) $(CXXFLAGS) -c -o $@ $< -I./$(HEA)

$(NAME)			:	$(OBJS)
					@ $(CXX) $(CXXFLAGS) -o $@ $(OBJS)

clean			:
					@ echo -e "$(YELLOW)$(BOLD)Cleaning files in $(BUI)...$(RESET)$(RED)"
					@ rm -rfv build
					@ echo -e -n "$(RESET)"

fclean			:	clean
					@ echo -e "$(YELLOW)$(BOLD)Cleaning program build...$(RESET)$(RED)"
					@ rm -fv $(NAME)
					@ rm -rfv $(BUI)

re				:	fclean all

.PHONY			:	all clean fclean re

-include $(DEPS)

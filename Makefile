NAME		=	ircserv
INC			=	./includes/
TEMPLAT		=	./templates/
BUILD		=	./build/
SRC			=	./src/


SRCS		=	main.cpp \
				Client/Client.cpp \
				Client/validators.cpp \
				Command/commandFactory/split.cpp \
				Command/commandFactory/commandFactory.cpp \
				Server/Server.cpp

OBJS		=	$(addprefix $(BUILD), $(SRCS:.cpp=.o))
DEPS		=	$(addprefix $(BUILD), $(SRCS:.cpp=.d))

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -MMD -fsanitize=address

all			:	$(NAME)

$(BUILD)%.o	:	$(SRC)%.cpp
				@ mkdir -p $(dir $@)
				@ $(CC) $(CFLAGS) -c -o $@ $< -I$(INC)

$(NAME)		:	$(OBJS)
				@ $(CXX) $(CFLAGS) -o $(NAME) $(OBJS)

clean		:
				rm -rf $(BUILD)

fclean		:	clean
				rm -f $(NAME)

re			:	fclean all

.PHONY		:	all clean fclean re

-include $(DEPS)

NAME			=	ircserv

CXX				=	c++
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++98
INCLUDES		=	-Iincludes

SRC_DIR			=	src
OBJ_DIR			=	build

SRC				=	$(SRC_DIR)/main.cpp \
					$(SRC_DIR)/Client/Client.cpp \
					$(SRC_DIR)/Server/Server.cpp


OBJ				=	$(OBJ:%.o=%.o)
OBJ				=	$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all				:	$(NAME)

$(NAME)			:	$(OBJ)
					$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o	:	$(SRC_DIR)/%.cpp
					@ mkdir -p $(dir $@)
					$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean			:
					rm -rf $(OBJ_DIR)

fclean			:	clean
					rm -f $(NAME)

re				: fclean all

.PHONY			: all clean fclean re

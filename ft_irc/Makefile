# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nachofluxa <nachofluxa@student.42.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/10 19:01:55 by nachofluxa        #+#    #+#              #
#    Updated: 2026/02/20 10:55:16 by nachofluxa       ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -Iincludes

SRC_DIR		= src
OBJ_DIR		= obj

SRC			= $(SRC_DIR)/main.cpp \
			$(SRC_DIR)/server.cpp \
			$(SRC_DIR)/check_argv.cpp


OBJ         = $(OBJ:%.o=%.o)
OBJ         = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aberneli <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/28 09:00:43 by aberneli          #+#    #+#              #
#    Updated: 2023/01/18 16:12:40 by aberneli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC := main.cpp Server.cpp User.cpp Channels.cpp \
	SocketIo.cpp Commands.cpp CommandsExt.cpp Utils.cpp \
	NumericReplies.cpp Config.cpp
	
OBJ = $(SRC:.cpp=.o)
HEADER = Server.hpp User.hpp Channels.hpp \
	SocketIo.hpp Utils.hpp NumericReplies.hpp \
	Modes.hpp Config.hpp

NAME = ircserv

FLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	c++ $(FLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp $(HEADER)
	c++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

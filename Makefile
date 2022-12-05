# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aberneli <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/28 09:00:43 by aberneli          #+#    #+#              #
#    Updated: 2022/11/30 19:12:20 by aberneli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC := main.cpp Server.cpp User.cpp Channels.cpp SocketIo.cpp Commands.cpp CommandsExt.cpp Utils.cpp NumericReplies.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = Irc.hpp Server.hpp User.hpp Channels.hpp SocketIo.hpp Utils.hpp NumericReplies.hpp

NAME = ircserv

MAKEFILEV = Makefile

FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

all: $(NAME)

$(NAME): $(OBJ) $(HEADER) $(MAKEFILEV)
	c++ $(FLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp $(HEADER) $(MAKEFILEV)
	c++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

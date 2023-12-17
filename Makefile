# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rennacir <rennacir@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/23 15:58:10 by rennacir          #+#    #+#              #
#    Updated: 2023/12/17 21:25:10 by rennacir         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FLAGS 			= -std=c++98 #-Wall -Werror -Wextra
NAME 			= irc
CC				= c++
RM				= rm -fr
FILES			=  Server.cpp Client.cpp Channel.cpp Bot.cpp botBonus.cpp ExecuteAll.cpp Invite.cpp Join.cpp Kick.cpp \
				   main.cpp Mode.cpp Nick.cpp Oper.cpp Part.cpp Pass.cpp Privmsg.cpp Quit.cpp Topic.cpp User.cpp
OBJECT_FILES	= $(FILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJECT_FILES)
	$(CC) $(FLAGS) $(OBJECT_FILES) -o $(NAME)

%.o:%.cpp Client.hpp Channel.hpp
	$(CC) $(FLAGS) -c $< -o $@
clean:
	@$(RM) $(OBJECT_FILES)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all fclean clean re
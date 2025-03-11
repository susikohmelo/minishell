# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lfiestas <lfiestas@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/10 17:13:50 by lfiestas          #+#    #+#              #
#    Updated: 2025/02/28 18:14:51 by ljylhank         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
SRCS = main.c array.c array_mem.c executor.c lexer.c parser.c \
	get_next_line.c get_next_line_utils.c execute_command.c \
	get_delimiter_input.c pipes_redirects.c apply_redirects.c builtins.c \
	apply_builtin.c execute_builtin.c builtin_exit_atoi.c signals.c wildcard.c \
	preprocessor.c
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

CC = cc
CFLAGS = -Wall -Wextra -Werror -D_GNU_SOURCE -Ilibft

MAKEFLAGS += -j6

.PHONY: all debug clean flcean re

all: $(NAME)

libft/libft.a:
	make -C libft

$(NAME): $(OBJS) libft/libft.a
	@mkdir -p build
	$(CC) -o $@ $^ $(CFLAGS) -lreadline

build/%.o: %.c minishell.h pipes_redirects.h get_next_line.h
	@mkdir -p build
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	make clean -C libft
	rm -rf build

fclean: clean
	make fclean -C libft
	rm -rf build $(NAME) tests

re:
	make fclean
	make all

debug: CFLAGS += -ggdb3 -gdwarf -D__int128_t=uint64_t
debug: CFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak
debug: $(NAME)

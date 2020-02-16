# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: osamoile <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/08/01 15:53:40 by osamoile          #+#    #+#              #
#    Updated: 2019/11/09 16:15:32 by osamoile         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NM_NAME =		ft_nm
HEADER =		./includes/nm.h
INC =			-I ./includes \
				-I ./libft/includes
LIB =			-lft -L ./libft
CC_FLAGS =		# TDCHECK: -Wall -Wextra -Werror
SRC_LIST =		main \
				archive
OBJ =			$(addprefix obj/, $(addsuffix .o, $(SRC_LIST)))

OFF=\033[0m
PURPLE=\033[0;38;2;102;102;255m
PINK=\033[0;35m
PINKBOLD=\033[1;35m
WHITEBOLD=\033[1;37m
RED=\033[0;31m
REDBOLD=\033[1;31m

all: $(NM_NAME)

$(NM_NAME): ./libft/libft.a obj $(OBJ)
	@gcc $(OBJ) -o $(NM_NAME) $(LIB)
	@echo "$(PINKBOLD)$(NM_NAME)$(PINK) ready$(OFF)"

./libft/libft.a:
	@make -C ./libft

obj:
	@mkdir obj

obj/%.o: src/%.c $(HEADER)
	@gcc $(CC_FLAGS) -c $< -o $@ $(INC)
	@echo "$(PURPLE)Compiling $(WHITEBOLD)$*.c $(PURPLE)done$(OFF)"

clean:
	@rm -rf obj
	@echo "$(RED)Directory $(REDBOLD)obj$(RED) removed$(OFF)"

fclean: clean
	@rm -f $(NM_NAME)
	@echo "$(REDBOLD)$(NM_NAME)$(RED) removed$(OFF)"

re: fclean all

debug: CC_FLAGS += -D DEBUG
debug: re

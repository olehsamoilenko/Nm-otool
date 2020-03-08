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
OTOOL_NAME =	ft_otool
HEADER =		./includes/nm.h
INC =			-I ./includes \
				-I ./libft/includes
ifeq ($(shell uname), Linux)
	INC += -I ./platform-include
endif
LFT =			-lft -L ./libft

CC_FLAGS =		# TDCHECK: -Wall -Wextra -Werror
SRC_LIST =		main \
				archive \
				fat \
				segment \
				load_command \
				symbol
NM_OBJ =		$(addprefix obj/nm_,	$(addsuffix .o, $(SRC_LIST)))
OTOOL_OBJ =		$(addprefix obj/otool_,	$(addsuffix .o, $(SRC_LIST)))

OFF=\033[0m
PURPLE=\033[0;38;2;102;102;255m
PINK=\033[0;35m
PINKBOLD=\033[1;35m
WHITEBOLD=\033[1;37m
RED=\033[0;31m
REDBOLD=\033[1;31m

all: $(NM_NAME) $(OTOOL_NAME)

$(NM_NAME): ./libft/libft.a obj $(NM_OBJ)
	@gcc $(NM_OBJ) -o $(NM_NAME) $(LFT)
	@echo "$(PINKBOLD)$(NM_NAME)$(PINK) ready$(OFF)"

$(OTOOL_NAME): ./libft/libft.a obj $(OTOOL_OBJ)
	@gcc $(OTOOL_OBJ) -o $(OTOOL_NAME) $(LFT)
	@echo "$(PINKBOLD)$(OTOOL_NAME)$(PINK) ready$(OFF)"

./libft/libft.a:
	@make -C ./libft

obj/nm_%.o: src/%.c $(HEADER)
	@gcc $(CC_FLAGS) -c $< -o $@ $(INC) -DNM
	@echo "$(PURPLE)Compiling $(WHITEBOLD)$*.c$(PURPLE) \
	for $(WHITEBOLD)ft_nm $(PURPLE)done$(OFF)"

obj/otool_%.o: src/%.c $(HEADER)
	@gcc $(CC_FLAGS) -c $< -o $@ $(INC) -DOTOOL
	@echo "$(PURPLE)Compiling $(WHITEBOLD)$*.c$(PURPLE) \
	for $(WHITEBOLD)ft_otool $(PURPLE)done$(OFF)"

obj:
	@mkdir obj

clean:
	@rm -rf obj
	@echo "$(RED)Directory $(REDBOLD)obj$(RED) removed$(OFF)"

fclean: clean
	@rm -f $(NM_NAME)
	@echo "$(REDBOLD)$(NM_NAME)$(RED) removed$(OFF)"
	@rm -f $(OTOOL_NAME)
	@echo "$(REDBOLD)$(OTOOL_NAME)$(RED) removed$(OFF)"

re: fclean all

debug: CC_FLAGS += -D DEBUG
debug: re

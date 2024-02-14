################################################################################

MAKE		= make --no-print-directory

NAME		= ft_ls

SRC_DIR		= src/
BUILD_DIR	= build/
LIB_DIR		= lib/

LIB			= $(LIB_DIR)libft
LINK		= -L $(LIB) -lft

CFLAGS		= -I $(LIB) -MMD -MP -MF $(BUILD_DIR)$*.d -Wall -Werror -Wextra -O3
RM			= rm -rf
CC			= gcc

################################################################################

-include $(SRC_DIR)src.mk

SRC			:=	$(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ 		=	$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(SRC:.c=.o))
DEP 		=	$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(SRC:.c=.d))

################################################################################

all:
	@$(MAKE) -C $(LIB)
	@$(MAKE) $(NAME)

$(BUILD_DIR)%.o: $(SRC_DIR)%.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME):: $(OBJ)
	@$(CC) $(CFLAGS) $(LINK) $(OBJ) -o $(NAME)
	@printf "$(basename $@): done\n"

clean:
	@$(MAKE) $@ -sC $(LIB)
	$(RM) $(BUILD_DIR)

fclean:
	@$(MAKE) $@ -sC $(LIB)
	@$(MAKE) clean
	$(RM) $(NAME)

re:
	@$(MAKE) fclean
	@$(MAKE)

.PHONY: all clean fclean re
-include $(DEP)

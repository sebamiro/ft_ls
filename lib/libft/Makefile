#
# smiro
# Makefile
# 2024-02
#

################################################################################

MAKE		= make --no-print-directory
NAME		= libft.a

SRC_DIR		= src/
BUILD_DIR	= build/

-include $(SRC_DIR)src.mk

CFLAGS		= -I ./$(SRC_DIR) -MMD -MP -MF $(BUILD_DIR)$*.d -Wall -Werror -Wextra -O3
RM			= rm -rf
CC			= gcc
AR			= ar rcs

################################################################################

SRC			= 	$(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ 		=	$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(SRC:.c=.o))
DEP 		=	$(patsubst $(SRC_DIR)%, $(BUILD_DIR)%, $(SRC:.c=.d))

################################################################################

all:
			@$(MAKE) $(NAME)

$(BUILD_DIR)%.o: $(SRC_DIR)%.c Makefile
			@mkdir -p $(@D)
			$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJ)
			$(AR) $(NAME) $(OBJ)

clean:
			$(RM) $(BUILD_DIR)

fclean: 	clean
			$(RM) $(NAME)

re:
			@$(MAKE) fclean
			@$(MAKE)

.PHONY: all clean fclean re
-include $(DEP)

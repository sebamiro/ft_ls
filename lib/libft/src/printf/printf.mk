#
# smiro
# printf.mk
# 2024-02
#

PRINTF_DIR = printf
PRINTF_INC = $(PRINTF_DIR)/printf.h

PRINTF_SRC = $(PRINTF_DIR)/printf.c \
			 $(PRINTF_DIR)/handletype.c \
			 $(PRINTF_DIR)/identcheck.c \
			 $(PRINTF_DIR)/printf.c \
			 $(PRINTF_DIR)/puthexnbr.c \
			 $(PRINTF_DIR)/putnbr.c \
			 $(PRINTF_DIR)/putptr.c



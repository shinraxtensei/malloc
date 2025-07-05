# Check HOSTTYPE environment variable as required by specification
ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -fPIC
LDFLAGS = -shared

# Directories
SRC_DIR = src
INCLUDE_DIR = includes

# Source files - only mandatory ones
SRCS = $(SRC_DIR)/malloc.c \
       $(SRC_DIR)/malloc_core.c \
       $(SRC_DIR)/malloc_helpers.c \
       $(SRC_DIR)/malloc_helpers2.c \
       $(SRC_DIR)/malloc_statistics.c \
       $(SRC_DIR)/malloc_utils.c \
       $(SRC_DIR)/zones.c \
       $(SRC_DIR)/blocks.c \
       $(SRC_DIR)/validation.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/show_alloc_mem.c

# Object files
OBJS = $(SRCS:.c=.o)

# Library names as required by specification
LIB_NAME = libft_malloc_$(HOSTTYPE).so
SYMLINK_NAME = libft_malloc.so

# Default target
all: $(LIB_NAME) $(SYMLINK_NAME)

# Build the library
$(LIB_NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Create symbolic link as required by specification
$(SYMLINK_NAME): $(LIB_NAME)
	ln -sf $(LIB_NAME) $(SYMLINK_NAME)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean object files
clean:
	rm -f $(OBJS)

# Clean all generated files
fclean: clean
	rm -f $(LIB_NAME) $(SYMLINK_NAME)

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re
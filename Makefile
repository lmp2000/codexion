NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = \
	src/main.c \
	src/parse.c \
	src/parse_utils.c \
	src/time.c \
	src/init.c \
	src/cleanup.c \
	src/log.c \
	src/simulation.c \
	src/coder.c \
	src/state.c \
	src/coder_state.c \
	src/monitor.c \
	src/dongle.c \
	src/dongle_access.c \
	src/heap.c \
	src/heap_order.c \
	src/heap_extra.c \
	src/scheduler.c \
	src/scheduler_order.c \
	src/scheduler_grant.c

OBJ = $(SRC:.c=.o)

INCLUDES = -I include

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c include/codexion.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

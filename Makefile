CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -D_POSIX_C_SOURCE=200809L

SRCS = src/main.c src/logger.c src/file_manager.c src/games.c
OBJS = $(SRCS:.c=.o)
TARGET = minios

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

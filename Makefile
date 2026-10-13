CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -std=c99 -D_POSIX_C_SOURCE=200809L
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/logger.c $(SRC_DIR)/file_manager.c $(SRC_DIR)/games.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = minios

.PHONY: all clean init

all: init $(TARGET)

init:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p disk

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

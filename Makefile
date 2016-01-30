SRC_DIR = src
HEADERS = $(SRC_DIR)/config_handler.h
OBJECTS = $(SRC_DIR)/config_handler.o $(SRC_DIR)/main.o
LIBS = -lncurses -lmenu
CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -std=gnu11

default: syslinux-editor

debug: CFLAGS += -DDEBUG
debug: syslinux-editor

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

syslinux-editor: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f syslinux-editor

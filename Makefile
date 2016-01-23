SRC_DIR = src
HEADERS = $(SRC_DIR)/config_handler.h
OBJECTS = $(SRC_DIR)/config_handler.o $(SRC_DIR)/main.o
LIBS = -lncurses -lmenu
CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic

default: config_editor

debug: CFLAGS += -DDEBUG
debug: config_editor

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

config_editor: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f $(SRC_DIR)/$(OBJECTS)
	-rm -f config_editor

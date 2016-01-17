HEADERS = config_handler.h
OBJECTS = config_handler.o main.o
LIBS = -lncurses -lmenu
CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic

default: config_editor

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

config_editor: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f program

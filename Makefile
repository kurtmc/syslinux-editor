SRC_DIR = src
HEADERS = $(SRC_DIR)/config_handler.h
OBJECTS = $(SRC_DIR)/config_handler.o $(SRC_DIR)/main.o
TEST = $(SRC_DIR)/config_handler.o $(SRC_DIR)/test.o
LIBS = -lncurses -lmenu
CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -std=gnu11

default: syslinux-editor

debug: CFLAGS += -DDEBUG
debug: clean test-suite syslinux-editor

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

syslinux-editor: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f syslinux-editor

install: default
	mkdir -p $(DESTDIR)/usr/bin/
	cp syslinux-editor $(DESTDIR)/usr/bin/

test-suite: $(TEST)
	$(CC) $(TEST) $(LIBS) -o $@
	./test-suite

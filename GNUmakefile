HIGHSCOREDIR := $(PREFIX)/games/2048

CC             ?= clang
CFLAGS         += -Wno-visibility -Wno-incompatible-pointer-types -Wall -Wextra
CFLAGS         += -DINVERT_COLORS -DVT100 -O2 -I/usr/local/include -DHIGHSCOREDIR=\"$(HIGHSCOREDIR)\"
LDFLAGS         += -L/usr/local/lib -lintl
PREFIX         ?= /usr/local
MANPREFIX      ?= $(PREFIX)/man/man6

PROGRAM := 2048
MANFILES := man/2048.6
C_FILES := $(wildcard src/*.c)
MERGE_FILE := src/merge_std.c
FILTERED_C_FILES := $(filter-out src/gfx%.c src/merge%.c, $(C_FILES))

all: terminal

curses: $(FILTERED_C_FILES) src/gfx_curses.c
	$(CC) $(CFLAGS) $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_curses.c -o $(PROGRAM) $(LDFLAGS) -lcurses

terminal: $(FILTERED_C_FILES) src/gfx_terminal.c
	$(CC) $(CFLAGS) $(FILTERED_C_FILES) $(MERGE_FILE) src/gfx_terminal.c -o $(PROGRAM) $(LDFLAGS)

remake: clean all

clean:
	rm -f $(PROGRAM)

install:
	cp -fv $(PROGRAM) $(PREFIX)/bin
	cp -fv $(MANFILES) $(MANPREFIX)
	mkdir -p $(HIGHSCOREDIR)
	chown root:games $(HIGHSCOREDIR)
	chmod 774 $(HIGHSCOREDIR)

uninstall:
	rm -fv $(PREFIX)/bin/$(PROGRAM)
	rm -fv $(MANPREFIX)/$(MANFILES)

.PHONY: clean remake install uninstall

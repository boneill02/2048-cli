.POSIX:
HIGHSCOREDIR := $(PREFIX)/games/2048
PREFIX         ?= /usr/local
MANPREFIX      ?= $(PREFIX)/man/man6

CC             ?= cc
CFLAGS         += -Wno-visibility -Wno-incompatible-pointer-types -Wall -Wextra
CFLAGS         += -DINVERT_COLORS -DVT100 -O2 -I$(PREFIX)/include -DHIGHSCOREDIR=\"$(HIGHSCOREDIR)\"
LDFLAGS        += -L$(PREFIX)/lib -lintl

PROGRAM := 2048
MANFILES := man/2048.6
C_FILES := src/ai.c src/engine.c src/gfx_curses.c src/gfx_terminal.c src/highscore.c src/main.c src/merge_fib.c src/merge_std.c src/options.c
MERGE_FILE := src/merge_std.c
FILTERED_C_FILES := src/ai.c src/engine.c src/highscore.c src/main.c src/options.c
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

.PHONY: all clean remake install uninstall

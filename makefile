CFLAGS  ?= -std=c99
LDFLAGS ?= -lncurses

all: spell

spell: game.c spell.c
	$(CC) $(CFLAGS) $+ $(LDFLAGS) -o $@

clean:
	rm -f spell

.PHONY: all clean

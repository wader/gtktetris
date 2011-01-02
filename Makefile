PROGRAM = gtktetris
GETTEXT_PACKAGE = gtktetris
OBJS = misc.o highscore.o tetris.o interface.o
BIN_PATH = /usr/local/bin
HIGHSCORE_PATH = /usr/local/games/gtktetris
GROUP = root
HIGHSCORE_FILE = $(HIGHSCORE_PATH)/highscore.dat
#SIZE = -DBIGGERBLOCKS
SIZE = -DBIGBLOCKS
CC = gcc

CFLAGS = `pkg-config gtk+-2.0 --cflags` -Wall -O2 \
	-DHIGHSCORE_FILE=\"$(HIGHSCORE_FILE)\" $(SIZE) 

LIBS = `pkg-config gtk+-2.0 --libs`

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM) $(LIBS)
	strip $(PROGRAM)

clean: 
	rm -f *.o *~ $(PROGRAM)

install:
	mkdir -p $(BIN_PATH)
	mkdir -p $(HIGHSCORE_PATH)
	install -o root -g $(GROUP) $(PROGRAM) $(BIN_PATH)
	if [ ! -f $(HIGHSCORE_FILE) ]; then \
		: > $(HIGHSCORE_FILE); \
	fi
	chown root.$(GROUP) $(HIGHSCORE_FILE)
	chmod 666 $(HIGHSCORE_FILE)
#	ln -s $(BIN_PATH)/$(PROGRAM) /usr/games/bin/tetris
uninstall:
	rm -i $(BIN_PATH)/$(PROGRAM)
	rm -i $(HIGHSCORE_FILE)
	rmdir $(HIGHSCORE_PATH)

dist:
	-cd .. ; tar c gtktetris-0.5 | gzip -9 > gtktetris-0.5.tar.gz

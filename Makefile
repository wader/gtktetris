PROGRAM = gtktetris
GETTEXT_PACKAGE = gtktetris
OBJS = misc.o highscore.o tetris.o interface.o
BIN_PATH = /usr/local/bin
#SIZE = -DBIGGERBLOCKS
SIZE = -DBIGBLOCKS
CC = gcc

CFLAGS = `pkg-config gtk+-2.0 --cflags` -Wall -O2 $(SIZE) 
LIBS = `pkg-config gtk+-2.0 --libs`

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM) $(LIBS)
	strip $(PROGRAM)

clean: 
	rm -f *.o *~ $(PROGRAM)

install:
	mkdir -p $(BIN_PATH)
	install $(PROGRAM) $(BIN_PATH)

uninstall:
	rm -i $(BIN_PATH)/$(PROGRAM)

pack:	
	rm -rf gtktetris.tgz
	tar -czvf gtktetris.tgz *.c *.h *.xpm Makefile README THANKS ChangeLog

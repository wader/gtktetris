include ../config.mk

APP  = gtktetris
# ls *.c | sed 's%\.c%.o%g'
# remove interface_menu.o (included by interface.c)
OBJS = draw.o highscore.o interface.o main.o options.o tetris.o

#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: $(APP)
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)

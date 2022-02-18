include ../config.mk

APP  = gtktetris
# ls *.c | sed 's%\.c%.o%g'
# remove interface_menu.o (included by interface.c)
OBJS = draw.o help.o highscore.o interface.o main.o options.o tetris.o

#CPPFLAGS += -I..
CFLAGS += -I..

CFLAGS += $(GTK_CFLAGS)
LIBS   += $(GTK_LIBS)

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

clean:
	rm -f *.o *~ $(APP) $(APP).exe

distclean: clean

install: $(APP)
	mkdir -p $(DESTDIR)$(bindir)
	install -c $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)

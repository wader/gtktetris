include ../config.mk

all:

strip:

install:
	mkdir -p $(DESTDIR)$(datadir)/applications
	install -c -m 644 gtktetris.desktop $(DESTDIR)$(datadir)/applications
	mkdir -p $(DESTDIR)$(datadir)/pixmaps
	install -c -m 644 gtktetris.png $(DESTDIR)$(datadir)/pixmaps

install-strip: install

uninstall:
	rm -f $(DESTDIR)$(datadir)/applications/gtktetris.desktop
	rm -f $(DESTDIR)$(datadir)/pixmaps/gtktetris.png

clean:

distclean:



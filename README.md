## GtkTetris

A simple Tetris clone for the GTK library.

## Requirements

- GTK >= 2.14 / GTK 3 >= 3.4

## Installation

Run

- ./autogen.sh
- ./configure --prefix=/usr
- make
- make install

By default the GTK3 port is compiled, use `--enable-gtk2`
or `--disable-gtk3` to compile the GTK2 port.

- ./configure --prefix=/usr --enable-gtk2

The GTK3 version provides HiDPI support with:

- `GDK_SCALE=2 ./gtktetris` - make UI 2x bigger
- `GDK_SCALE=3 ./gtktetris` - make UI 3x bigger

You can also just run the compiled binary: src/gtktetris.

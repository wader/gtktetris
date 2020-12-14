## GtkTetris

A simple Tetris clone for the GTK library.

## Requirements

- GTK 2.14+

## Installation

Run

- ./autogen.sh
- ./configure --prefix=/usr
- make
- make install

To build the GTK3 port:

- ./configure --prefix=/usr --enable-gtk3

The GTK3 version provides HiDPI support with:

- `GDK_SCALE=2 ./gtktetris` - make UI 2x bigger
- `GDK_SCALE=3 ./gtktetris` - make UI 3x bigger

You can also just run the compiled binary: src/gtktetris.

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

To build the GTK3 port (currently broken):

- ./configure --prefix=/usr --enable-gtk3

You can also just run the compiled binary: src/gtktetris.
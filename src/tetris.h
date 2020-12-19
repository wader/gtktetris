/*
 * common includes
 */

#ifndef _TETRIS_H__
#define _TETRIS_H__

#include <gtk/gtk.h>
#include "gtkcompat.h"
#include <stdlib.h>

#define MAX_X 10
#define MAX_Y 18
#define do_random(max) ( (int) ((max) * ((float)random() / RAND_MAX)) )

extern int BLOCK_WIDTH;
extern int BLOCK_HEIGHT;
extern GtkWidget * main_window;

#define NUM_LEVELS 20
#define NUM_HIGHSCORE 10

#define START_LEVEL_VALUE 6 // Iavor

// global variables
GtkWidget *game_area;
GtkWidget *next_block_area;
int game_over;
int game_pause;
int current_x;
int current_y;
int current_block;
int current_frame;
long current_score;
int current_level;
int current_lines;
int next_block;
int next_frame;

// options.c
char * get_config_dir_file (const char * file);
void options_defaults (void);
void options_read(void);
void options_show_dialog (void);

struct Options
{
  int start_level;
  int noise_level;
  int noise_height;
  int show_next_block;
};

struct Options options;

// tetris.c
void draw_block(int x,int y,int block,int frame,int clear,int next);
void to_virtual();
void from_virtual();
int valid_position(int x,int y,int block,int frame);
void move_block(int x,int y,int f);
int check_lines();
int move_down();
void new_block();
void game_over_clean();
void game_init();
void make_noise(int level,int height);

// interface.c
void update_game_values();
int game_loop();
void game_over_init();
void game_set_pause();

// draw.c
void load_tetris_blocks (const char ** source_blocks_pix);
void free_tetris_blocks (void);
void set_block(int x,int y,int color,int next);
void set_background_color (cairo_t * cr, GtkWidget * widget);

// highscore.c
void read_highscore();
void write_highscore();
void show_highscore(int place);
void check_highscore();
int addto_highscore(char *name,long score, int level, int lines);

#endif

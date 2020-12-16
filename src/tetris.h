#ifndef _TETRIS_H__
#define _TETRIS_H_

#include <gtk/gtk.h>
#include "gtkcompat.h"

#define MAX_X 10
#define MAX_Y 18

extern int BLOCK_WIDTH;
extern int BLOCK_HEIGHT;
extern GtkWidget * main_window;

#define NUM_LEVELS 20
#define NUM_HIGHSCORE 10

#define START_LEVEL_VALUE 6 // Iavor

// global variables
GtkWidget *game_area;
GtkWidget *next_block_area;
GdkPixbuf *blocks_pixbuf;
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
void options_defaults (void);
void options_read(void);
void options_show_dialog (void);

struct Options
{
  int level;
  int noise_l;
  int noise_h;
  gint shw_nxt;
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

// misc.c
void set_block(int x,int y,int color,int next);
int do_random(int max);
void set_label_with_color (GtkWidget * w, char * color, char * text);
char * get_config_dir_file (const char * file);
GtkWidget *label_box (GtkWidget *parent, GtkWidget *label, gchar *label_text);

// highscore.c
void read_highscore();
void write_highscore();
void show_highscore(int place);
void check_highscore();
int addto_highscore(char *name,long score, int level, int lines);

#endif

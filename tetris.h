#ifndef _TETRIS_H__
#define _TETRIS_H_

#include <gtk/gtk.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define MAX_X 10
#define MAX_Y 18

#ifdef BIGBLOCKS
#define BLOCK_WIDTH 22
#define BLOCK_HEIGHT 22
#else
#ifdef BIGGERBLOCKS
#define BLOCK_WIDTH 33
#define BLOCK_HEIGHT 33
#else
#define BLOCK_WIDTH 15
#define BLOCK_HEIGHT 15
#endif
#endif

#define NUM_LEVELS 20
#define NUM_HIGHSCORE 10

#define START_LEVEL_VALUE 6 // Iavor

// global variables
GtkWidget *game_area;
GtkWidget *next_block_area;
GdkPixmap *blocks_pixmap;
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
//int show_next_block;

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
void set_label(GtkWidget *label,char *str);
void add_submenu(gchar *name,GtkWidget *menu,GtkWidget *menu_bar,int right);
GtkWidget *add_menu_item(gchar *name,GtkSignalFunc func,gpointer data,gint state,GtkWidget *menu);
GtkWidget *add_menu_item_toggle(gchar *name,GtkSignalFunc func,gpointer data,gint state,GtkWidget *menu);
void set_gtk_color_style(GtkWidget *w, long red, long green,  long blue);
void get_opt_file(char *buf, int len);
GtkWidget *label_box (GtkWidget *parent, GtkWidget *label, gchar *label_text);

// highscore.c
void read_highscore();
void write_highscore();
void show_highscore(int place);
void check_highscore();
int addto_highscore(char *name,long score, int level, int lines);

#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "tetris.h"

#define EMPTY_STRING ""

void set_block(int x,int y,int color,int next)
{
// need to figure out how to replace this usage of gdk_draw_pixbuf
#if ! GTK_CHECK_VERSION (3, 0, 0)
	gdk_draw_pixbuf (
			(!next ? gtk_widget_get_window (game_area) : gtk_widget_get_window (next_block_area)),
			NULL,
			blocks_pixbuf,        /* pixbuf */
			color*BLOCK_WIDTH, 0, /* src_x, src_y */
			x*BLOCK_WIDTH,     y*BLOCK_HEIGHT, /* dest_x, dest_y */
			BLOCK_WIDTH,       BLOCK_HEIGHT,   /* width,  height */
			0,0,0);
#endif
}

int do_random(int max)
{
	return max*((float)random()/RAND_MAX);
}

void set_label_with_color (GtkWidget * w, char * color, char * text)
{
   char * markup;
   markup = g_markup_printf_escaped ("<span foreground=\"%s\">%s</span>",
                                     color, text);
   gtk_label_set_markup (GTK_LABEL (w), markup);
   g_free (markup);
}

/* returns a path that must be freed with g_free) */
char * get_config_dir_file (const char * file)
{
   char * config_home = NULL;
   char * res = NULL;
   config_home = getenv ("XDG_CONFIG_HOME");
   if (!config_home) {
      config_home = getenv ("HOME");
      if (!config_home) {
         config_home = EMPTY_STRING;
      }
   }
   if (file) {
      res = g_strconcat (config_home, "/gtktetris/", file, NULL);
   } else {
      res = g_strconcat (config_home, "/gtktetris", NULL);
   }
   return (res);
}


GtkWidget *label_box (GtkWidget *parent, GtkWidget *label, gchar *label_text)
     {
         GtkWidget *box1;

         /* create box for label */
         box1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
         gtk_container_set_border_width (GTK_CONTAINER (box1), 2);

         /* create label for button */
         gtk_label_set_text (GTK_LABEL(label), label_text);

         /* pack the label into the box */
         gtk_box_pack_start (GTK_BOX (box1), label, TRUE, TRUE, 3);
         gtk_widget_show(label);

         return (box1);
     }


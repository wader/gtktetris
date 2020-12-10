#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>

#include "tetris.h"

void set_block(int x,int y,int color,int next)
{
	gdk_draw_drawable((!next ? game_area->window : next_block_area->window),
			(!next ? game_area->style->black_gc : next_block_area->style->black_gc),
			blocks_pixmap,
			color*BLOCK_WIDTH,0,
			x*BLOCK_WIDTH,y*BLOCK_HEIGHT,
			BLOCK_WIDTH,BLOCK_HEIGHT);
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

void get_opt_file(char *buf, int len)
{
  struct passwd *pw;

  pw = getpwuid(getuid());
  strcpy(buf,pw->pw_dir);
  strcat(buf,"/.gtktetris\0");
}

GtkWidget *label_box (GtkWidget *parent, GtkWidget *label, gchar *label_text)
     {
         GtkWidget *box1;

         /* create box for label */
         box1 = gtk_hbox_new (FALSE, 0);
         gtk_container_set_border_width (GTK_CONTAINER (box1), 2);

         /* create label for button */
         gtk_label_set_text (GTK_LABEL(label), label_text);

         /* pack the label into the box */
         gtk_box_pack_start (GTK_BOX (box1), label, TRUE, TRUE, 3);
         gtk_widget_show(label);

         return (box1);
     }


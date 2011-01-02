#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>

#include "tetris.h"

void set_block(int x,int y,int color,int next)
{
	gdk_draw_pixmap((!next ? game_area->window : next_block_area->window),
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

void set_label(GtkWidget *label,char *str)
{
	gtk_label_set(GTK_LABEL(label), str);
}


void set_gtk_color_style(GtkWidget *w, long red, long green,  long blue)
{
	GtkStyle *style;
	GdkColormap *colormap;
	GdkColor color;

	style = gtk_style_new();
	colormap = gdk_colormap_get_system();
	color.red = red;
	color.green = green;
	color.blue = blue;
	gdk_color_alloc(colormap,&color);
	memcpy(style->fg,&color,sizeof(GdkColor));
	gtk_widget_set_style(w,style);
	gtk_style_unref(style);
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
         GtkStyle *style;

         /* create box for label */
         box1 = gtk_hbox_new (FALSE, 0);
         gtk_container_border_width (GTK_CONTAINER (box1), 2);

         /* get style of button. */
         style = gtk_widget_get_style(parent);

         /* create label for button */
         gtk_label_set (GTK_LABEL(label), label_text);

         /* pack the label into the box */

         gtk_box_pack_start (GTK_BOX (box1), label, TRUE, TRUE, 3);
         gtk_widget_show(label);

         return (box1);
     }


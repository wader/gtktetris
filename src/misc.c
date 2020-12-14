#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "tetris.h"

#define EMPTY_STRING ""

static void //  https://stackoverflow.com/a/36483677 
_cairo_gdk_draw_pixbuf (cairo_t *cr, cairo_surface_t *source,
                        int src_x,   int src_y,
                        int dest_x,  int dest_y,
                        int width,   int height)
{
   cairo_save (cr);

   /* Move (0, 0) to the destination position */
   cairo_translate (cr, dest_x, dest_y);

   /* Set up the source surface in such a way that
    * (src_x, src_y) maps to (0, 0) in user coordinates. */
   cairo_set_source_surface (cr, source, -src_x, -src_y);

   /* Do the drawing */
   cairo_rectangle (cr, 0, 0, width, height);
   cairo_fill (cr);

   /* Undo all of our modifications to the drawing state */
   cairo_restore (cr);
}


void set_block(int x,int y,int color,int next)
{
   int src_x, src_y, dest_x, dest_y, width, height;
   cairo_t * cr;
   GdkWindow * gdkwin;

   src_x = color*BLOCK_WIDTH;
   src_y = 0;
   dest_x = x*BLOCK_WIDTH;
   dest_y = y*BLOCK_HEIGHT;
   width  = BLOCK_WIDTH;
   height = BLOCK_HEIGHT;

   if (next) {
      gdkwin = gtk_widget_get_window (next_block_area);
   } else {
      gdkwin = gtk_widget_get_window (game_area);
   }
   cr = gdk_cairo_create (gdkwin);
   gdk_cairo_set_source_pixbuf (cr,
                                blocks_pixbuf,
                                src_x,
                                src_y);

   // This a special case where we don't need
   //   the whole pixbuf, just a region.
   // It's a bit complicated with cairo,
   //   so we need the source's surface to perform operations.
   // See
   //   https://developer.gnome.org/gdk2/stable/gdk2-Drawing-Primitives.html#gdk-draw-drawable
   cairo_pattern_t * pattern;
   cairo_surface_t * surface;
   pattern = cairo_get_source (cr);
   cairo_pattern_get_surface (pattern, &surface);
   _cairo_gdk_draw_pixbuf (cr, surface,
                           src_x,  src_y,
                           dest_x, dest_y,
                           width,  height);

   cairo_destroy (cr);
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


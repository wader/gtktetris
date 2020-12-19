/* 
 * draw on surfaces with Cairo 
 * - interface.c: provides cairo context and/or widget
 * - tetris.c   : calls set_block
 */

#include "tetris.h"

// blocks.xpm contains 8 blocks in a single img - 8 colors
// block 0 = black / background
// 1.....7 = block colors

static cairo_t * cr_blocks[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL } ;
static GdkPixbuf * blocks_pixbuf;


void set_background_color (cairo_t * cr, GtkWidget * widget)
{
   int width  = gtk_widget_get_allocated_width (widget);
   int height = gtk_widget_get_allocated_height (widget);
   cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
   cairo_rectangle (cr,
                    0, 0,
                    width, height);
   cairo_fill (cr);
}


static void //  https://stackoverflow.com/a/36483677 
_cairo_gdk_draw_pixbuf (cairo_t *cr,
                        int src_x,   int src_y,
                        int dest_x,  int dest_y,
                        int width,   int height)
{
   // This a special case where we don't need
   //   the whole pixbuf, just a region.
   // It's a bit complicated with cairo,
   //   so we need the source's surface to perform operations.
   // See
   //   https://developer.gnome.org/gdk2/stable/gdk2-Drawing-Primitives.html#gdk-draw-drawable
   cairo_pattern_t * pattern;
   cairo_surface_t * source;
   pattern = cairo_get_source (cr);
   cairo_pattern_get_surface (pattern, &source);

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


void load_tetris_blocks (const char ** source_blocks_pix)
{
   int orig_block_width, orig_block_height;
   int src_x, src_y, dest_x, dest_y, width, height;
   int i;
   cairo_surface_t * surface;

   blocks_pixbuf = gdk_pixbuf_new_from_xpm_data (source_blocks_pix);
   orig_block_height = gdk_pixbuf_get_height (blocks_pixbuf);
   orig_block_width  = orig_block_height;

   // set global variables
   BLOCK_HEIGHT = orig_block_height;
   BLOCK_WIDTH  = orig_block_width;

   // allocate memory for the blocks and extract them from the source pix
   for (i = 0; i < 8; i++)
   {
      width = orig_block_width;
      height = orig_block_height;
      src_x = i * width;
      src_y = 0;
      dest_x = 0;
      dest_y = 0;

      surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, width, height);
      cr_blocks[i] = cairo_create (surface);
      cairo_surface_destroy (surface);

      gdk_cairo_set_source_pixbuf (cr_blocks[i], blocks_pixbuf,
                                   src_x, src_y);
      _cairo_gdk_draw_pixbuf (cr_blocks[i],
                              src_x,  src_y,
                              dest_x, dest_y,
                              width, height);
   }
}


void free_tetris_blocks (void)
{
   int i;
   for (i = 0; i < 8; i++)
   {
      if (cr_blocks[i]) {
         cairo_destroy (cr_blocks[i]);
         cr_blocks[i] = NULL;
      }
   }
}


void set_block(int x,int y,int color,int next)
{
   int dest_x, dest_y;
   cairo_t * cr;
   GdkWindow * gdkwin;

   dest_x = x*BLOCK_WIDTH;
   dest_y = y*BLOCK_HEIGHT;

   if (next) {
      gdkwin = gtk_widget_get_window (next_block_area);
   } else {
      gdkwin = gtk_widget_get_window (game_area);
   }

   cr = gdk_cairo_create (gdkwin);

   // get surface from cr_blocks[color]
   cairo_surface_t * source;
   source = cairo_get_target (cr_blocks[color]);

   // draw on target x,y. source already has correct width and height
   cairo_set_source_surface (cr, source, dest_x, dest_y);
   cairo_paint (cr);

   cairo_destroy (cr);
}


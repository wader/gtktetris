/* 
 * draw on surfaces with Cairo 
 * - interface.c: provides cairo context and/or widget
 * - tetris.c   : calls set_block
 */

#include "tetris.h"

// blocks.xpm contains 8 blocks in a single img - 8 colors
// block 0 = black / background
// 1.....7 = block colors

static cairo_surface_t * tetris_block[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL } ;
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


static void
_cairo_resize_surface (cairo_surface_t ** in_surface, int new_width, int new_height)
{
   cairo_t * new_cr;
   cairo_surface_t * out_surface;
   cairo_format_t format;
   int in_w, in_h;
   double scale_x, scale_y;

   // get current width x height
   in_w = cairo_image_surface_get_width (*in_surface);
   in_h = cairo_image_surface_get_height (*in_surface);

   if (in_w == new_width && in_h == new_height) {
      // no need to resize
      return;
   }

   // create a surface with the desired width x height
   format = cairo_image_surface_get_format (*in_surface);
   out_surface = cairo_image_surface_create (format, new_width, new_height);
   new_cr  = cairo_create (out_surface);

   // determine scale_x and scale_y
   scale_x = (double) new_width  / (double) in_w;
   scale_y = (double) new_height / (double) in_h;

   // scale and draw on out_surface
   cairo_scale (new_cr, scale_x, scale_y);
   cairo_set_source_surface (new_cr, *in_surface, 0, 0);
   cairo_paint (new_cr);
   cairo_destroy (new_cr);

   // set out_surface as the new surface
   cairo_surface_destroy (*in_surface);
   *in_surface = out_surface;
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
   float orig_block_width, orig_block_height;
   float src_x, src_y, dest_x, dest_y, width, height;
   int i;
   cairo_t * cr;
   cairo_format_t format;

   blocks_pixbuf = gdk_pixbuf_new_from_xpm_data (source_blocks_pix);
   orig_block_height = gdk_pixbuf_get_height (blocks_pixbuf);
   orig_block_width  = orig_block_height;

   if (gdk_pixbuf_get_n_channels (blocks_pixbuf) == 3) {
      format = CAIRO_FORMAT_RGB24;
   } else {
      format = CAIRO_FORMAT_ARGB32;
   }

   // allocate memory for the blocks and extract them from the source pix
   for (i = 0; i < 8; i++)
   {
      width = orig_block_width;
      height = orig_block_height;
      src_x = i * width;
      src_y = 0;
      dest_x = 0;
      dest_y = 0;

      tetris_block[i] = cairo_image_surface_create (format, width, height);
      cr = cairo_create (tetris_block[i]);

      gdk_cairo_set_source_pixbuf (cr, blocks_pixbuf,
                                   src_x, src_y);
      _cairo_gdk_draw_pixbuf (cr,
                              src_x,  src_y,
                              dest_x, dest_y,
                              width, height);
      // may need to resize the block
      _cairo_resize_surface (&tetris_block[i], BLOCK_WIDTH, BLOCK_HEIGHT);

      cairo_destroy (cr);
   }
}


void free_tetris_blocks (void)
{
   int i;
   for (i = 0; i < 8; i++)
   {
      if (tetris_block[i]) {
         cairo_surface_destroy (tetris_block[i]);
         tetris_block[i] = NULL;
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

   // draw on target x,y. source already has correct width and height
   cairo_set_source_surface (cr, tetris_block[color], dest_x, dest_y);
   cairo_paint (cr);

   cairo_destroy (cr);
}


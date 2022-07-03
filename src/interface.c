/*
 *  gtk interface to tetris.c
 * 
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tetris.h"
#include "tetris.xpm" /* tetris_xpm */

#undef PACKAGE_NAME
#define PACKAGE_NAME "GtkTetris"

static void show_about (void);
static void game_start_stop (void);
static void menu_start (void);
static void menu_stop (void);
static void menu_set_pause (void);
#if GTK_CHECK_VERSION (3, 10, 0)
static void create_menu_bar (GtkApplicationWindow * window, GtkApplication * app);
#else
static void create_menu_bar (GtkBox * box, GtkWindow * window);
#endif
#include "interface_menu.c"

#include "blocks.xpm"
int BLOCK_WIDTH;
int BLOCK_HEIGHT;

int game_play;
char *pause_str[2]={"Pause\0","Resume\0"};
char *start_stop_str [2] = { "Start\0", "Stop\0"};
GtkWidget * main_window;

GtkWidget * score_label;
GtkWidget * hiscore_label;
GtkWidget * level_label;
GtkWidget * lines_label;
GtkWidget *Start_stop_button;
GtkWidget *Start_stop_button_label;
GtkWidget *Pause_button;
GtkWidget *Pause_button_label;
GtkWidget *about_window;
gint timer;

int level_speeds[NUM_LEVELS] = {
    1000,886,785,695,616,546,483,428,379,336,298,
    264,234,207,183,162,144,127,113,100
};

static void set_label_with_color (GtkWidget * w, char * color, char * text)
{
    char * markup;
    markup = g_markup_printf_escaped ("<b><span foreground=\"%s\">%s</span></b>",
                                      color, text);
    gtk_label_set_markup (GTK_LABEL (w), markup);
    g_free (markup);
}

void update_game_values()
{
    char dummy[20] = "";
    snprintf (dummy, sizeof(dummy), "%lu", current_score);
    set_label_with_color (score_label, "red", dummy);
    snprintf (dummy, sizeof(dummy), "%d", current_level);
    set_label_with_color (level_label, "blue", dummy);
    snprintf (dummy, sizeof(dummy), "%d", current_lines);
    set_label_with_color (lines_label, "green", dummy);
    snprintf (dummy, sizeof(dummy), "%ld", get_hiscore ());
    set_label_with_color (hiscore_label, "black", dummy);
}


gint keyboard_event_handler (GtkWidget *widget, GdkEventKey *event,
                             gpointer data)
{
    int dropbonus = 0;
  
    if(game_over || game_pause) {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_KEY(Escape):
            game_set_pause ();
            return TRUE;
            break;
        case GDK_KEY(w):
        case GDK_KEY(W):
        case GDK_KEY(Up):
            move_block(0,0,1);
            event->keyval=0;
            return TRUE;
            break;
        case GDK_KEY(x):
        case GDK_KEY(X):
        case GDK_KEY(z):
        case GDK_KEY(Z):
            move_block(0,0,-1); 
            event->keyval=0; 
            return TRUE;
            break;
        case GDK_KEY(s):
        case GDK_KEY(S):
        case GDK_KEY(Down):
            move_down(); 
            event->keyval=0; 
            return TRUE;
            break;
        case GDK_KEY(a):
        case GDK_KEY(A):
        case GDK_KEY(Left):
            move_block(-1,0,0); 
            event->keyval=0; 
            return TRUE;
            break;
        case GDK_KEY(d):
        case GDK_KEY(D):
        case GDK_KEY(Right):
            move_block(1,0,0); 
            event->keyval=0; 
            return TRUE;
            break;
        case GDK_KEY(space):
            while(move_down()) {
                dropbonus++;
            }
            current_score += dropbonus*(current_level+1);
            update_game_values();
            event->keyval=0;
            return TRUE;
            break;
    }
    return FALSE;
}


static gboolean
game_area_draw_cb (GtkWidget * widget, gpointer compat, gpointer user_data)
{
    if (!game_over)
    {
        from_virtual();
        move_block(0,0,0); 
    }
    else
    {
#if GTK_CHECK_VERSION (3, 0, 0)
        cairo_t * cr = (cairo_t *) compat;
        GdkRectangle rect;
        GdkRectangle * area = &rect;
        gdk_cairo_get_clip_rectangle (cr, area);
#else // gtk2
        GdkEventExpose * event = (GdkEventExpose *) compat;
        cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
        GdkRectangle * area = &(event->area);
#endif
        set_background_color (cr, area);
#if GTK_MAJOR_VERSION == 2
        cairo_destroy (cr);
#endif
    }
    return (FALSE);
}


static gboolean
next_block_area_draw_cb (GtkWidget * widget, gpointer compat, gpointer user_data)
{
#if GTK_CHECK_VERSION (3, 0, 0)
    cairo_t * cr = (cairo_t *) compat;
    GdkRectangle rect;
    GdkRectangle * area = &rect;
    gdk_cairo_get_clip_rectangle (cr, area);
#else // gtk2
    GdkEventExpose * event = (GdkEventExpose *) compat;
    cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
    GdkRectangle * area = &(event->area);
#endif
    set_background_color (cr, area);
#if GTK_MAJOR_VERSION == 2
    cairo_destroy (cr);
#endif
    if(!game_over && options.show_next_block) {
        draw_block(0,0,next_block,next_frame,FALSE,TRUE);
    }
    return FALSE;
}


int game_loop()
{
    if(!game_over)
    {
        timer = g_timeout_add (level_speeds[current_level],
                               (GSourceFunc)game_loop, NULL);
        move_down();
    }
    return FALSE;
}


void game_set_pause (void)
{
    if (game_over) {
        return;
    }
    game_pause = !game_pause;
    if (timer) {
        g_source_remove (timer);
        timer = 0;
    }
    if (game_pause) {
        gtk_label_set_text (GTK_LABEL(Pause_button_label),pause_str[1]);
    } else {
        timer = g_timeout_add (level_speeds[current_level],
                               (GSourceFunc)game_loop, NULL);
        gtk_label_set_text (GTK_LABEL (Pause_button_label), pause_str[0]);
    }
}

void game_over_init()
{
    int high_dummy;
    char hscr[30];
    if(current_score && (high_dummy = addto_highscore((char *)getenv("USER"),current_score,current_level,current_lines)))
    {
        write_highscore();
    }

    snprintf (hscr, sizeof(hscr), "%ld", get_hiscore ());
    set_label_with_color (hiscore_label, "black", hscr);

    game_over = TRUE;
    game_play = FALSE;

    gtk_widget_queue_draw (game_area);
    gtk_widget_queue_draw (next_block_area);

    gtk_label_set_text (GTK_LABEL (Start_stop_button_label), start_stop_str[0]);
    menu_stop ();
    gtk_widget_set_sensitive (Start_stop_button,TRUE);
    gtk_widget_grab_default (Start_stop_button);
    gtk_label_set_text (GTK_LABEL(Pause_button_label),pause_str[0]);
    gtk_widget_set_sensitive (Pause_button,FALSE);

    if (timer) {
        g_source_remove (timer);
        timer = 0;
    }
}


static void game_start_stop (void)
{
    game_play = !game_play;
    if (game_play)
    {
        menu_start ();
        gtk_widget_set_sensitive (Start_stop_button, TRUE);
        gtk_label_set_text (GTK_LABEL(Start_stop_button_label),start_stop_str[1]);
        gtk_widget_set_sensitive(Pause_button,TRUE);
        gtk_widget_grab_default(Pause_button);
        game_init();
        make_noise(options.noise_level,options.noise_height);
        from_virtual();
        move_block(0,0,0);
        current_level = options.start_level;
        timer = g_timeout_add(level_speeds[current_level],(GSourceFunc)game_loop,NULL);
    } else {
        game_over_init();
    }
}

static void show_about (void)
{
    GtkWidget * w;
    GdkPixbuf * logo;
    const gchar * authors[] =
    {
        "1999-2000 Mattias Wadman",
        "2002-2006 Iavor Veltchev",
        "2020-2022 wdlkmpx (github)",
        NULL
    };
    logo = gdk_pixbuf_new_from_xpm_data (tetris_xpm);

    w = g_object_new (GTK_TYPE_ABOUT_DIALOG,
                      "version",      VERSION,
                      "program-name", PACKAGE_NAME,
                      "copyright",    "Copyright (C) 1999-2021",
                      "comments",     "Just another GTK Tetris",
                      "license",      "MIT - Permission is hereby granted, free of charge, \nto any person obtaining a copy of this software \nand associated documentation files (the \"Software\"), \nto deal in the Software without restriction, \nincluding without limitation the rights to use, \ncopy, modify, merge, publish, distribute, sublicense, \nand/or sell copies of the Software.... \n\nsee LICENSE file",
                      "website",      PACKAGE_URL,
                      "authors",      authors,
                      "logo",         logo,
                      NULL);
    gtk_container_set_border_width (GTK_CONTAINER (w), 2);
    gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (main_window));
    gtk_window_set_modal (GTK_WINDOW (w), TRUE);
    gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_CENTER_ON_PARENT);

    g_signal_connect_swapped (w, "response",
                              G_CALLBACK (gtk_widget_destroy), w);
    gtk_widget_show_all (GTK_WIDGET (w));
}


void update_block_size (int startup)
{
    int resize = 0;
    int resize_main_window = 0;

    // determine if need to resize blocks and game area
    if (startup) {
        resize = 1;
    } else {
        if (BLOCK_HEIGHT != options.block_size) {
            resize = 1;
        }
        if (options.block_size < BLOCK_HEIGHT) {
            resize_main_window = 1;
        }
    }

    BLOCK_HEIGHT = options.block_size;
    BLOCK_WIDTH  = options.block_size;

    if (resize)
    {
        // allocate blocks
        free_tetris_blocks ();
        load_tetris_blocks (blocks_xpm);

        // set game_area and next_block_area size
        gtk_widget_set_size_request (GTK_WIDGET (game_area),
                                     MAX_X * BLOCK_WIDTH,
                                     MAX_Y * BLOCK_HEIGHT);
        gtk_widget_set_size_request (GTK_WIDGET (next_block_area),
                                     4 * BLOCK_WIDTH,
                                     4 * BLOCK_HEIGHT);
        if (resize_main_window) {
            // https://stackoverflow.com/questions/8903140/shrink-window-in-gtk-dynamically-when-content-shrinks
            // GTK3: the automatic resizing of windows only happens 
            //       when elements are too large to be drawn
            // So we need to make the window smaller manually
#if GTK_CHECK_VERSION (3, 0, 0)
            gtk_window_resize (GTK_WINDOW (main_window),
                               MAX_X * BLOCK_WIDTH + 4 * BLOCK_WIDTH,
                               MAX_Y * BLOCK_HEIGHT);
#endif
        }
    }
}


static void main_window_destroy_cb (GtkWidget * w, gpointer   user_data)
{ // terminate application
    gtktetris_exit ();
}

void create_main_window (void)
{
    GtkWidget *v_box;
    GtkWidget *h_box;
    GtkWidget * frame_labels, * vbox_labels, * box_space;
    GtkWidget *right_side;
    GtkWidget * label;

    game_play=FALSE;
    game_over = TRUE;
    game_pause = FALSE;
    current_x = current_y = 0;
    current_block = current_frame = 0;
    current_score = current_lines = 0;
    current_level = options.start_level; 
    next_block = next_frame = 0;
    // seed random generator
    srandom(time(NULL));

    // window
    main_window = gtk_application_window_new (gtktetris_app);
    gtk_window_set_resizable (GTK_WINDOW (main_window), FALSE);
    gtk_window_set_title(GTK_WINDOW(main_window),"GTK Tetris");
    g_signal_connect (G_OBJECT (main_window), "key_press_event",
                      G_CALLBACK (keyboard_event_handler), NULL);
    g_signal_connect (G_OBJECT (main_window), "destroy",
                    G_CALLBACK (main_window_destroy_cb), NULL);

    GdkPixbuf * icon = gdk_pixbuf_new_from_xpm_data (tetris_xpm);
    gtk_window_set_icon (GTK_WINDOW(main_window), icon);
    g_object_unref (icon);

    // vertical box
    v_box = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
    gtk_container_add(GTK_CONTAINER(main_window),v_box);

    // menu bar
#if GTK_CHECK_VERSION (3, 10, 0)
    create_menu_bar (GTK_APPLICATION_WINDOW (main_window), gtktetris_app);
#else
    create_menu_bar (GTK_BOX (v_box), GTK_WINDOW (main_window));
#endif

   // horizontal box
   h_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   gtk_box_pack_start (GTK_BOX (v_box), h_box, FALSE, FALSE, 2);
  
    // game_area
    game_area = gtk_drawing_area_new();
    g_signal_connect (game_area, GTKCOMPAT_DRAW_SIGNAL,
                      G_CALLBACK (game_area_draw_cb), NULL);
    gtk_widget_set_events (game_area, GDK_EXPOSURE_MASK);
    gtk_box_pack_start (GTK_BOX (h_box), game_area, FALSE, FALSE, 2);
  
    // right_side
    right_side = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
    gtk_box_pack_start (GTK_BOX(h_box), right_side, FALSE, FALSE, 3);
  
    // next_block_area
    next_block_area = gtk_drawing_area_new();
    g_signal_connect (next_block_area, GTKCOMPAT_DRAW_SIGNAL,
                      G_CALLBACK (next_block_area_draw_cb), NULL);
    gtk_widget_set_events(next_block_area, GDK_EXPOSURE_MASK);
    gtk_box_pack_start (GTK_BOX (right_side), next_block_area, FALSE, FALSE, 0);
  
    // the score, level and lines labels
    frame_labels = gtk_frame_new (NULL);
    gtk_box_pack_start (GTK_BOX (right_side), frame_labels, FALSE, FALSE, 5);
    vbox_labels = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (frame_labels), vbox_labels);
    gtk_container_set_border_width (GTK_CONTAINER (vbox_labels), 5); /* padding inside frame */
  
    label = gtk_label_new ("Score:");
    gtk_box_pack_start (GTK_BOX (vbox_labels), label, FALSE, FALSE, 0);
    score_label = gtk_label_new ("0");
    gtk_box_pack_start (GTK_BOX (vbox_labels), score_label, FALSE, FALSE, 0);

    box_space = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox_labels), box_space, FALSE, FALSE, 7);

    label = gtk_label_new("Level:");
    gtk_box_pack_start (GTK_BOX (vbox_labels), label, FALSE, FALSE, 0);
    level_label = gtk_label_new ("0");
    gtk_box_pack_start (GTK_BOX (vbox_labels), level_label, FALSE, FALSE, 0);

    box_space = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox_labels), box_space, FALSE, FALSE, 7);

    label = gtk_label_new ("Lines:");
    gtk_box_pack_start (GTK_BOX (vbox_labels), label, FALSE, FALSE, 0);
    lines_label = gtk_label_new ("0");
    gtk_box_pack_start (GTK_BOX (vbox_labels), lines_label, FALSE, FALSE, 0);

    box_space = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox_labels), box_space, FALSE, FALSE, 7);

    label = gtk_label_new ("High-score:");
    gtk_box_pack_start (GTK_BOX (vbox_labels), label, FALSE, FALSE, 0);
    hiscore_label = gtk_label_new ("0");
    gtk_box_pack_start (GTK_BOX (vbox_labels), hiscore_label, FALSE, FALSE, 0);

    //the game buttons
    //Start_stop
    Start_stop_button = gtk_button_new_with_label (start_stop_str[0]);
    Start_stop_button_label = gtk_bin_get_child (GTK_BIN (Start_stop_button));
    g_signal_connect (Start_stop_button, "clicked",
                      G_CALLBACK (game_start_stop), NULL);

    gtk_box_pack_start(GTK_BOX(right_side),Start_stop_button,FALSE,FALSE,3);
    gtk_widget_set_can_default (Start_stop_button, TRUE);
    gtk_widget_grab_default(Start_stop_button);

    //Pause
    Pause_button = gtk_button_new_with_label (pause_str[0]);
    Pause_button_label = gtk_bin_get_child (GTK_BIN (Pause_button));
    g_signal_connect (Pause_button, "clicked",
                      G_CALLBACK (menu_set_pause), NULL);
    gtk_box_pack_start(GTK_BOX(right_side),Pause_button,FALSE,FALSE,3);
    gtk_widget_set_can_default (Pause_button, TRUE);
    gtk_widget_set_sensitive (Pause_button,FALSE);

    read_highscore ();
    update_game_values ();

    gtk_widget_show_all (main_window);

    // set block size and game area size
    update_block_size (1);
}


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "tetris.h"

#ifdef BIGBLOCKS
#include "big_blocks.xpm"
#else
#ifdef BIGGERBLOCKS
#include "bbig_blocks.xpm"
#else
#include "blocks.xpm"
#endif
#endif

int game_play;
char options_f[100];
char *pause_str[2]={"Pause\0","Resume\0"};
char *start_stop_str[2]={"Start Game\0","Stop game\0"};
GtkWidget *score_label1;
GtkWidget *score_label2;
GtkWidget *level_label1;
GtkWidget *level_label2;
GtkWidget *lines_label1;
GtkWidget *lines_label2;
GtkWidget *menu_game_quick;
GtkWidget *menu_game_start;
GtkWidget *menu_game_stop;
GtkWidget *menu_game_quit;
GtkWidget *menu_game_pause;
GtkWidget *menu_game_show_next_block;
GtkWidget *menu_save_options;
GtkWidget *spin_level;
GtkWidget *spin_noise_level;
GtkWidget *spin_noise_height;
GtkWidget *new_game_window;
GtkWidget *Start_stop_button;
GtkWidget *Start_stop_button_label;
GtkWidget *Pause_button;
GtkWidget *Pause_button_label;
GtkWidget *help_window;
GtkWidget *about_window;
gint timer;

int level_speeds[NUM_LEVELS] = {1000,886,785,695,616,546,483,428,379,336,298,
				264,234,207,183,162,144,127,113,100};	

void update_game_values()
{
	char dummy[20] = "";

	sprintf(dummy,"%lu",current_score);
	set_label(score_label2,dummy);
	sprintf(dummy,"%d",current_level);
	set_label(level_label2,dummy);
	sprintf(dummy,"%d",current_lines);
	set_label(lines_label2,dummy);
}

gint keyboard_event_handler(GtkWidget *widget,
			    GdkEventKey *event,
			    gpointer data)
{
  int dropbonus = 0;
  
  if(game_over || game_pause)
    return FALSE;
  switch(event->keyval)
    {
    case GDK_x: case GDK_X:
      move_block(0,0,1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_w: case GDK_W: case GDK_Up: 
      move_block(0,0,-1); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_s: case GDK_S:  
      move_down(); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_a: case GDK_A: case GDK_Left: 
      move_block(-1,0,0); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_d: case GDK_D: case GDK_Right: 
      move_block(1,0,0); 
      event->keyval=0; 
      return TRUE;
      break;
    case GDK_space: case GDK_Down:
      while(move_down())
	dropbonus++;
      current_score += dropbonus*(current_level+1);
      update_game_values();
      event->keyval=0;
       return TRUE;
     break;
    }
  return FALSE;
}

void game_show_next_block(GtkMenuItem     *menuitem,
			  gpointer         user_data)
{
  options.shw_nxt = !options.shw_nxt;
  if(!game_over) {
    if(!options.shw_nxt)
      draw_block(0,0,next_block,next_frame,TRUE,TRUE);
    else
      draw_block(0,0,next_block,next_frame,FALSE,TRUE);
  }
}

gint game_area_expose_event(GtkWidget       *widget,
			    GdkEventExpose  *event,
			    gpointer         user_data)
{	
  if(!game_over)
    {
      from_virtual();
      move_block(0,0,0); 
    }
  else
    gdk_draw_rectangle(widget->window,
		       widget->style->black_gc,
		       TRUE,
		       0,0,
		       widget->allocation.width,
		       widget->allocation.height);
  return FALSE;
}

gboolean next_block_area_expose_event(GtkWidget       *widget,
				      GdkEventExpose  *event,
				      gpointer         user_data)
{
	gdk_draw_rectangle(widget->window,
		widget->style->black_gc,
		TRUE,
		0,0,
		widget->allocation.width,
		widget->allocation.height);
	if(!game_over && options.shw_nxt)
		draw_block(0,0,next_block,next_frame,FALSE,TRUE);
	return FALSE;
}

int game_loop()
{
	if(!game_over)
	{
	  timer = gtk_timeout_add(level_speeds[current_level],
				  (GtkFunction)game_loop,NULL);
	  move_down();
	}
	return FALSE;
}

void game_set_pause_b()
{
  if(game_pause) 
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_game_pause),FALSE);
  else 
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_game_pause),TRUE);
  return;
}

void game_set_pause(GtkWidget    *menuitem,
		    gpointer         user_data)
{
   if (game_over)
    {
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_game_pause),
				     FALSE);
      return;
    }
  game_pause = !game_pause;
  if(game_pause) {
    gtk_timeout_remove(timer);
    gtk_label_set(GTK_LABEL(Pause_button_label),pause_str[1]);
  }
  else {
    timer = gtk_timeout_add(level_speeds[current_level],
			    (GtkFunction)game_loop,NULL);
    gtk_label_set(GTK_LABEL(Pause_button_label),pause_str[0]);
  }
}

void game_over_init()
{
	int high_dummy;
	read_highscore();
	if(current_score && (high_dummy = addto_highscore((char *)getenv("USER"),current_score,current_level,current_lines)))
	{
		write_highscore();
		show_highscore(high_dummy);
	}
	
	game_over = TRUE;
	game_play = FALSE;
	gdk_draw_rectangle(game_area->window,
		game_area->style->black_gc,
		TRUE,
		0,0,
		game_area->allocation.width,
		game_area->allocation.height);

	gdk_draw_rectangle(next_block_area->window,
		next_block_area->style->black_gc,
		TRUE,
		0,0,
		next_block_area->allocation.width,
		next_block_area->allocation.height);
	game_set_pause(GTK_WIDGET(menu_game_pause),NULL);
	gtk_label_set(GTK_LABEL(Start_stop_button_label),start_stop_str[0]);
	gtk_widget_set_sensitive(menu_game_quick,TRUE);
	gtk_widget_set_sensitive(menu_game_start,TRUE);
	gtk_widget_set_sensitive(menu_game_stop,FALSE);
	gtk_widget_set_sensitive(Start_stop_button,TRUE);
	gtk_widget_grab_default(Start_stop_button);
	gtk_label_set(GTK_LABEL(Pause_button_label),pause_str[0]);
	gtk_widget_set_sensitive(Pause_button,FALSE);
	
	gtk_timeout_remove(timer);
}

void game_start_stop(GtkMenuItem     *widget,
		     gpointer user_data)
{
  game_play=!game_play;
  gtk_widget_set_sensitive(GTK_WIDGET(widget), FALSE);
  if(game_play)
    {
      gtk_widget_set_sensitive(menu_game_stop,TRUE);
      gtk_widget_set_sensitive(menu_game_quick,FALSE);
      gtk_widget_set_sensitive(menu_game_start,FALSE);
      gtk_widget_set_sensitive(Start_stop_button,TRUE);
      gtk_label_set(GTK_LABEL(Start_stop_button_label),start_stop_str[1]);
      gtk_widget_set_sensitive(Pause_button,TRUE);
      gtk_widget_grab_default(Pause_button);
      game_init();
      make_noise(options.noise_l,options.noise_h);
      from_virtual();
      move_block(0,0,0);
      current_level = options.level;
      update_game_values(0,current_level,0);
      timer = gtk_timeout_add(level_speeds[current_level],(GtkFunction)game_loop,NULL);
    }
  else
    game_over_init();
}

void about_close(){
  gtk_widget_hide(about_window);}

void show_about(GtkMenuItem     *menuitem,
		gpointer         user_data)
{
        GtkWidget *About_close_button;
	GtkWidget *about_label;
	GtkWidget *about_border;
	GtkWidget *v_box;
	
	about_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(about_window),"About");
	gtk_window_set_policy(GTK_WINDOW(about_window),FALSE,FALSE,TRUE);
	gtk_window_set_position(GTK_WINDOW(about_window),GTK_WIN_POS_CENTER);
	gtk_container_border_width(GTK_CONTAINER(about_window),1);
	
	about_border = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(about_border),GTK_SHADOW_OUT);
	gtk_container_add(GTK_CONTAINER(about_window),about_border);
	
	v_box = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(about_border),v_box);
		
	about_label = gtk_label_new(	"\nJust another GTK Tetris v0.6.2\n\n"
					"(c)1999,2000 Mattias Wadman\n\n"
					"Modified by Iavor Veltchev, 2002-2006\n\n"
					"This program is distributed under the terms of GPL.\n");
	gtk_box_pack_start(GTK_BOX(v_box),about_label,FALSE,FALSE,0);

	About_close_button = gtk_button_new_with_label("Close");	
	g_signal_connect ((gpointer) About_close_button, "clicked",
			  G_CALLBACK (about_close),
			  NULL);
	gtk_box_pack_start(GTK_BOX(v_box),About_close_button,FALSE,TRUE,0);
  	GTK_WIDGET_SET_FLAGS(About_close_button, GTK_CAN_DEFAULT);
    	gtk_widget_grab_default(About_close_button);
	
	gtk_widget_show_all(about_window);
}


void help_close(){
  gtk_widget_hide(help_window);}

void show_help(GtkMenuItem     *menuitem,
	       gpointer         user_data)
{
        GtkWidget *Help_close_button;
	GtkWidget *help_label;
	GtkWidget *help_border;
	GtkWidget *hbox;
	GtkWidget *vbox;

	help_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(help_window),"Help");
	gtk_window_set_policy(GTK_WINDOW(help_window),FALSE,FALSE,TRUE);
	gtk_window_set_position(GTK_WINDOW(help_window),GTK_WIN_POS_CENTER);
	gtk_container_border_width(GTK_CONTAINER(help_window),1);
	
	help_border = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(help_border),GTK_SHADOW_OUT);
	gtk_container_add(GTK_CONTAINER(help_window),help_border);

	vbox = gtk_vbox_new(FALSE,3);
	gtk_container_add(GTK_CONTAINER(help_border),vbox);

	hbox = gtk_hbox_new(FALSE,30);
	gtk_container_add(GTK_CONTAINER(vbox),hbox);

	help_label = gtk_label_new(	"\nKeys:\n"
					"Right and \"d\"\n"
					"Left and \"a\"\n"
					"\"s\"\n"
					"Up and \"w\"\n"
					"\"x\"\n"
					"Space ans Down\n\n"
					"Score: score*level\n"
					"Single\n"
					"Double\n"
					"Triple\n"
					"TETRIS\n\n"
					"Drop bonus: rows*level\n");
	gtk_misc_set_alignment(GTK_MISC(help_label),0,0);	
	gtk_label_set_justify(GTK_LABEL(help_label),GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(hbox),help_label,TRUE,TRUE,TRUE);

	help_label = gtk_label_new(	"\n\n"
					"move right\n"
					"move left\n"
					"move down\n"
					"rotate ccw\n"
					"rotate cw\n"
					"drop block\n\n\n"
					"40\n100\n"
					"300\n1200\n");
	gtk_misc_set_alignment(GTK_MISC(help_label),0,0);	
	gtk_label_set_justify(GTK_LABEL(help_label),GTK_JUSTIFY_LEFT);
	gtk_box_pack_start(GTK_BOX(hbox),help_label,TRUE,TRUE,TRUE);

	Help_close_button = gtk_button_new_with_label("Close");	
	g_signal_connect ((gpointer) Help_close_button, "clicked",
			  G_CALLBACK (help_close),
			  NULL);
	gtk_box_pack_start(GTK_BOX(vbox),Help_close_button,FALSE,TRUE,0);
  	GTK_WIDGET_SET_FLAGS(Help_close_button, GTK_CAN_DEFAULT);
    	gtk_widget_grab_default(Help_close_button);

	
	gtk_widget_show_all(help_window);
}

void game_new_accept()
{
  options.level = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_level));
  options.noise_l = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_noise_level));
  options.noise_h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_noise_height));
  current_level = options.level;
  gtk_widget_set_sensitive(menu_game_start,TRUE);
  gtk_widget_set_sensitive(menu_game_quick,TRUE);
  update_game_values(0,current_level,0);
  gtk_widget_hide(new_game_window);
}

void game_new_wrapper()
{
  options.level = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_level));
  options.noise_l = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_noise_level));
  options.noise_h = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_noise_height));

  gtk_widget_set_sensitive(menu_game_quick,FALSE);
  gtk_widget_set_sensitive(menu_game_start,FALSE);
  gtk_widget_set_sensitive(menu_game_stop,TRUE);
  game_play=!game_play;
  gtk_label_set(GTK_LABEL(Start_stop_button_label),start_stop_str[1]);
  gtk_widget_set_sensitive(Pause_button,TRUE);
  gtk_widget_grab_default(Pause_button);			 
  game_init();
  make_noise(options.noise_l,options.noise_h);
  from_virtual();
  move_block(0,0,0);
  current_level = options.level;
  update_game_values(0,current_level,0);
  timer = gtk_timeout_add(level_speeds[current_level],(GtkFunction)game_loop,NULL);
  gtk_widget_hide(new_game_window);
}

void show_new_game_close(int close)
{
  gtk_widget_set_sensitive(menu_game_start,TRUE);
  gtk_widget_set_sensitive(menu_game_quick,TRUE);
  if(close)
    gtk_widget_hide(new_game_window);
}


void show_new_game(GtkMenuItem     *menuitem,
		   gpointer         user_data)
{
  GtkWidget *label;
  GtkWidget *frame;
  GtkWidget *vbox,*hbox;
  GtkWidget *table;
  GtkWidget *button;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkAdjustment *adj;
	
  new_game_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect ((gpointer) new_game_window, "destroy",
                    G_CALLBACK (show_new_game_close),
                    GINT_TO_POINTER(FALSE));
  
  gtk_window_set_title(GTK_WINDOW(new_game_window),"Level settings");
  gtk_window_set_position(GTK_WINDOW(new_game_window),GTK_WIN_POS_CENTER);
  gtk_container_border_width(GTK_CONTAINER(new_game_window),3);
  
  vbox = gtk_vbox_new(FALSE,2);
  gtk_container_add(GTK_CONTAINER(new_game_window),vbox);
  
  frame = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(vbox),frame,TRUE,TRUE,TRUE);
  
  table = gtk_table_new(3,2,TRUE);
  gtk_container_add(GTK_CONTAINER(frame),table);
  
  label = gtk_label_new("Start level:");
  adj = (GtkAdjustment *)gtk_adjustment_new(options.level,0,
					    NUM_LEVELS-1,1,1,0);
  spin_level = gtk_spin_button_new(adj,0,0);	
  gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
  gtk_table_attach_defaults(GTK_TABLE(table),spin_level,1,2,0,1);
  
  label = gtk_label_new("Noise level:");
  adj = (GtkAdjustment *)gtk_adjustment_new(options.noise_l,0,MAX_X-1,1,1,0);
  spin_noise_level = gtk_spin_button_new(adj,0,0);	
  gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,1,2);
  gtk_table_attach_defaults(GTK_TABLE(table),spin_noise_level,1,2,1,2);
  
  label = gtk_label_new("Noise height:");
  adj = (GtkAdjustment *)gtk_adjustment_new(options.noise_h,0,MAX_Y-4,1,1,0);
  spin_noise_height = gtk_spin_button_new(adj,0,0);	
  gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,2,3);
  gtk_table_attach_defaults(GTK_TABLE(table),spin_noise_height,1,2,2,3);
  
  hbox = gtk_hbox_new(TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,TRUE,0);
  
  button = gtk_button_new_with_label("Play");	
  g_signal_connect ((gpointer) button, "clicked",
                    G_CALLBACK (game_new_wrapper),
                    NULL);
  gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,TRUE,0);
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
  
  button1 = gtk_button_new_with_label("Accept");	
  g_signal_connect ((gpointer) button1, "clicked",
                    G_CALLBACK (game_new_accept),
                    NULL);
  gtk_box_pack_start(GTK_BOX(hbox),button1,FALSE,TRUE,0);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);
  
  button2 = gtk_button_new_with_label("Close");	
  g_signal_connect ((gpointer) button2, "clicked",
                    G_CALLBACK (show_new_game_close),
                    GINT_TO_POINTER(TRUE));
  gtk_box_pack_start(GTK_BOX(hbox),button2,FALSE,TRUE,0);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);
  
  gtk_widget_set_usize(new_game_window,220,130);
  gtk_widget_show_all(new_game_window);
  gtk_widget_set_sensitive(menu_game_start,FALSE);
  gtk_widget_set_sensitive(menu_game_quick,FALSE);
}

void show_highscore_wrapper(GtkMenuItem     *menuitem,
			    gpointer         user_data)
{
	read_highscore();
	show_highscore(0);
}

void save_options(GtkMenuItem     *menuitem,
		  gpointer         user_data)
{
  FILE *fp;
  if(!(fp = fopen(options_f,"wb"))){
    printf("gtktetris: Write ERROR!\n");
    return;}
    
  fwrite(&options,1,sizeof(options),fp);
  fclose(fp);
}

void read_options()
{	
  FILE *fp;
  if((fp = fopen(options_f,"rb")))
    {
      fread(&options,1,sizeof(options),fp);
      fclose(fp);
    }
}


int main(int argc,char *argv[])
{
  char dmmy[20];
  GtkWidget *main_window;
  GtkWidget *v_box;
  GtkWidget *h_box;
  GtkWidget *box1;
  GtkWidget *box2;
  GtkWidget *right_side;
  GtkWidget *game_border;	
  GtkWidget *next_block_border;
  GdkBitmap *mask;	
  GtkWidget *menu_bar;
  GtkWidget *menu_game;
  GtkWidget *menu_game_menu;
  GtkWidget *separatormenuitem1;
  GtkWidget *separator1;
  GtkWidget *menu_settings;
  GtkWidget *menu_settings_menu;
  GtkWidget *menu_help;
  GtkWidget *menu_help_menu;
  GtkWidget *help1;
  GtkWidget *high_scores1;
  GtkWidget *separator2;
  GtkWidget *about1;
  GtkAccelGroup* accel_group;
  

  //init game values
  game_play=FALSE;
  get_opt_file(options_f,100);
  read_options();
  game_over = TRUE;
  game_pause = FALSE;
  current_x = current_y = 0;
  current_block = current_frame = 0;
  current_score = current_lines = 0;
  current_level = options.level; 
  next_block = next_frame = 0;
  // seed random generator
  srandom(time(NULL));
  //options.shw_nxt = TRUE;
  
  gtk_set_locale();
  gtk_init(&argc,&argv);

  accel_group = gtk_accel_group_new();
  
  // window
  main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_policy(GTK_WINDOW(main_window),FALSE,FALSE,TRUE);
  gtk_window_set_title(GTK_WINDOW(main_window),"GTK Tetris");
  g_signal_connect ((gpointer) main_window, "key_press_event",
		    G_CALLBACK (keyboard_event_handler),
		    NULL);
  
  
  // vertical box
  v_box = gtk_vbox_new(FALSE,0);
  gtk_container_add(GTK_CONTAINER(main_window),v_box);
  gtk_widget_show(v_box);
  
  // menu stuff
  menu_bar = gtk_menu_bar_new();
  gtk_widget_show(menu_bar);
  gtk_box_pack_start(GTK_BOX(v_box),menu_bar,FALSE,FALSE,0);
  
  //Game sub-menu
  menu_game=gtk_menu_item_new_with_mnemonic ("_Game");
  gtk_widget_show(menu_game);
  gtk_container_add (GTK_CONTAINER (menu_bar), menu_game);
  
  menu_game_menu=gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_game), menu_game_menu);
  
  menu_game_quick = gtk_menu_item_new_with_mnemonic ("Start Game");
  gtk_widget_show (menu_game_quick);
  gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_quick);
  g_signal_connect ((gpointer) menu_game_quick, "activate",
		    G_CALLBACK (game_start_stop),
		    NULL);
  gtk_widget_add_accelerator (menu_game_quick, "activate", accel_group,
			      GDK_G, GDK_CONTROL_MASK,
			      GTK_ACCEL_VISIBLE);
  
  menu_game_stop = gtk_menu_item_new_with_mnemonic ("Stop Game");
  gtk_widget_show (menu_game_stop);
  gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_stop);
  g_signal_connect ((gpointer) menu_game_stop, "activate",
		    G_CALLBACK (game_start_stop),
		    NULL);
  gtk_widget_add_accelerator (menu_game_stop, "activate", accel_group,
			      GDK_O, GDK_CONTROL_MASK,
			      GTK_ACCEL_VISIBLE);
  gtk_widget_set_sensitive(menu_game_stop,FALSE);
  
  menu_game_pause = gtk_check_menu_item_new_with_mnemonic ("Pause");
  gtk_widget_show (menu_game_pause);
  gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_pause);
  g_signal_connect ((gpointer) menu_game_pause, "activate",
		    G_CALLBACK (game_set_pause),
		    NULL);
  gtk_widget_add_accelerator (menu_game_pause, "activate", accel_group,
			      GDK_P, GDK_CONTROL_MASK,
			      GTK_ACCEL_VISIBLE);
  
  separatormenuitem1 = gtk_menu_item_new ();
  gtk_widget_show (separatormenuitem1);
  gtk_container_add (GTK_CONTAINER (menu_game_menu), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);
  
  menu_game_quit = gtk_menu_item_new_with_mnemonic ("Quit");
  gtk_widget_show (menu_game_quit);
  gtk_container_add (GTK_CONTAINER (menu_game_menu), menu_game_quit);
  g_signal_connect ((gpointer) menu_game_quit, "activate",
		    G_CALLBACK (gtk_main_quit),
		    NULL);
  gtk_widget_add_accelerator(menu_game_quit,"activate", accel_group,
			     GDK_X, GDK_CONTROL_MASK,
			     GTK_ACCEL_VISIBLE);	
  
  //Settings sub-menu
  menu_settings = gtk_menu_item_new_with_mnemonic ("_Settings");
  gtk_widget_show (menu_settings);
  gtk_container_add (GTK_CONTAINER (menu_bar), menu_settings);
  
  menu_settings_menu=gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_settings), 
			     menu_settings_menu);
  
  menu_game_start = gtk_menu_item_new_with_mnemonic ("Level Settings");
  gtk_widget_show (menu_game_start);
  gtk_container_add (GTK_CONTAINER (menu_settings_menu), menu_game_start);
  g_signal_connect ((gpointer) menu_game_start, "activate",
		    G_CALLBACK (show_new_game),
		    NULL);
  
  menu_game_show_next_block = gtk_check_menu_item_new_with_mnemonic ("Show next block");
  gtk_widget_show (menu_game_show_next_block);
  gtk_container_add (GTK_CONTAINER (menu_settings_menu), 
		     menu_game_show_next_block);
  if (options.shw_nxt) 
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menu_game_show_next_block), 
				    TRUE);
  g_signal_connect ((gpointer) menu_game_show_next_block, "activate",
		    G_CALLBACK (game_show_next_block),
		    NULL);
  gtk_widget_add_accelerator (menu_game_show_next_block, "activate", 
			      accel_group,
			      GDK_N, GDK_CONTROL_MASK,
			      GTK_ACCEL_VISIBLE);
  
  
  separator1 = gtk_menu_item_new ();
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (menu_settings_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);
  
  menu_save_options = gtk_menu_item_new_with_mnemonic ("Save Settings");
  gtk_widget_show (menu_save_options);
  gtk_container_add (GTK_CONTAINER (menu_settings_menu), 
		     menu_save_options);
  g_signal_connect ((gpointer) menu_save_options, "activate",
		    G_CALLBACK (save_options),
		    NULL);
  
  //Help sub-menu
  menu_help=gtk_menu_item_new_with_mnemonic ("_Help");
  gtk_widget_show (menu_help);
  gtk_container_add (GTK_CONTAINER (menu_bar), menu_help);
  
  gtk_menu_item_set_right_justified (GTK_MENU_ITEM(menu_help),TRUE);
  
  menu_help_menu = gtk_menu_new();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_help), 
			     menu_help_menu);
  
  help1 = gtk_menu_item_new_with_mnemonic ("Help");
  gtk_widget_show (help1);
  gtk_container_add (GTK_CONTAINER (menu_help_menu), help1);
  g_signal_connect ((gpointer) help1, "activate",
		    G_CALLBACK (show_help),
		    NULL);
  gtk_widget_add_accelerator (help1, "activate", 
			      accel_group,
			      GDK_F1, (GdkModifierType) 0,
			      GTK_ACCEL_VISIBLE);
  
  high_scores1 = gtk_menu_item_new_with_mnemonic ("High-scores");
  gtk_widget_show (high_scores1);
  gtk_container_add (GTK_CONTAINER (menu_help_menu), high_scores1);
  g_signal_connect ((gpointer) high_scores1, "activate",
		    G_CALLBACK (show_highscore_wrapper),
		    NULL);
  
  separator2 = gtk_menu_item_new ();
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_help_menu), separator2);
  gtk_widget_set_sensitive (separator2, FALSE);
  
  about1 = gtk_menu_item_new_with_mnemonic ("About");
  gtk_widget_show (about1);
  gtk_container_add (GTK_CONTAINER (menu_help_menu), about1);
  g_signal_connect ((gpointer) about1, "activate",
		    G_CALLBACK (show_about),
		    NULL);
  
  // horizontal box
  h_box = gtk_hbox_new(FALSE,1);
  gtk_widget_show(h_box);
  gtk_box_pack_start(GTK_BOX(v_box),h_box,FALSE,FALSE,0);
  
  // game_border
  game_border = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(game_border),GTK_SHADOW_IN);
  gtk_box_pack_start(GTK_BOX(h_box),game_border,FALSE,FALSE,1);
  gtk_widget_show(game_border);
  
  // game_area
  game_area = gtk_drawing_area_new();
  gtk_widget_show(game_area);
  gtk_drawing_area_size(GTK_DRAWING_AREA(game_area),
			MAX_X*BLOCK_WIDTH,MAX_Y*BLOCK_HEIGHT);
  g_signal_connect ((gpointer) game_area, "expose_event",
		    G_CALLBACK (game_area_expose_event),
		    NULL);
  
  gtk_widget_set_events(game_area, GDK_EXPOSURE_MASK);
  gtk_container_add(GTK_CONTAINER(game_border),game_area);
  
  // right_side
  right_side = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(h_box),right_side,FALSE,FALSE,0);
  gtk_widget_show(right_side);
  
  // next_block_border
  next_block_border = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(next_block_border),GTK_SHADOW_IN);
  gtk_box_pack_start(GTK_BOX(right_side),next_block_border,FALSE,FALSE,0);
  gtk_widget_show(next_block_border);
  
  // next_block_area
  next_block_area = gtk_drawing_area_new();
  gtk_widget_show(next_block_area);
  gtk_drawing_area_size(GTK_DRAWING_AREA(next_block_area),
			4*BLOCK_WIDTH,4*BLOCK_HEIGHT);
  g_signal_connect ((gpointer) next_block_area, "expose_event",
		    G_CALLBACK (next_block_area_expose_event),
		    NULL);
  gtk_widget_set_events(next_block_area, GDK_EXPOSURE_MASK);
  gtk_container_add(GTK_CONTAINER(next_block_border),next_block_area);
  
  // the score,level and lines labels
  score_label1 = gtk_label_new("Score:");
  gtk_label_set_justify(GTK_LABEL(score_label1),GTK_JUSTIFY_RIGHT);
  gtk_widget_show(score_label1);
  gtk_box_pack_start(GTK_BOX(right_side),score_label1,FALSE,FALSE,3);
  
  score_label2 = gtk_label_new("0");
  set_gtk_color_style(score_label2,0xffff,0,0);
  gtk_label_set_justify(GTK_LABEL(score_label2),GTK_JUSTIFY_RIGHT);
  gtk_widget_show(score_label2);
  gtk_box_pack_start(GTK_BOX(right_side),score_label2,FALSE,FALSE,3);
  
  level_label1 = gtk_label_new("Level:");
  gtk_label_set_justify(GTK_LABEL(level_label1),GTK_JUSTIFY_RIGHT);
  gtk_widget_show(level_label1);
  gtk_box_pack_start(GTK_BOX(right_side),level_label1,FALSE,FALSE,3);
  
  sprintf(dmmy,"%d",current_level);
  level_label2 = gtk_label_new(dmmy);
  set_gtk_color_style(level_label2,0,0,0xffff);
  gtk_label_set_justify(GTK_LABEL(level_label2),GTK_JUSTIFY_RIGHT);
  gtk_widget_show(level_label2);
  gtk_box_pack_start(GTK_BOX(right_side),level_label2,FALSE,FALSE,3);
  
  lines_label1 = gtk_label_new("Lines:");
  gtk_label_set_justify(GTK_LABEL(lines_label1),GTK_JUSTIFY_RIGHT);
  gtk_widget_show(lines_label1);
  gtk_box_pack_start(GTK_BOX(right_side),lines_label1,FALSE,FALSE,3);
  
  lines_label2 = gtk_label_new("0");
  gtk_label_set_justify(GTK_LABEL(lines_label2),GTK_JUSTIFY_RIGHT);
  gtk_widget_show(lines_label2);
  gtk_box_pack_start(GTK_BOX(right_side),lines_label2,FALSE,FALSE,3);
  
  //the game buttons
  //Start_stop
  Start_stop_button = gtk_button_new();
  gtk_widget_show(Start_stop_button);
  g_signal_connect ((gpointer) Start_stop_button, "clicked",
		    G_CALLBACK (game_start_stop),
		    NULL);
  Start_stop_button_label= gtk_label_new(start_stop_str[0]);
  box2 = label_box(right_side, Start_stop_button_label, 
		   start_stop_str[0] );
  gtk_widget_show(box2);
  gtk_container_add (GTK_CONTAINER (Start_stop_button), box2);
  gtk_box_pack_start(GTK_BOX(right_side),Start_stop_button,FALSE,FALSE,3);
  GTK_WIDGET_SET_FLAGS(Start_stop_button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default(Start_stop_button);
  //Pause
  Pause_button = gtk_button_new();
  gtk_widget_show(Pause_button);
  g_signal_connect ((gpointer) Pause_button, "clicked",
		    G_CALLBACK (game_set_pause_b),
		    NULL);
  Pause_button_label = gtk_label_new(pause_str[0]);
  box1 = label_box(right_side, Pause_button_label, pause_str[0] );
  gtk_widget_show(box1);
  gtk_container_add (GTK_CONTAINER (Pause_button), box1);
  gtk_box_pack_start(GTK_BOX(right_side),Pause_button,FALSE,FALSE,3);
  GTK_WIDGET_SET_FLAGS(Pause_button, GTK_CAN_DEFAULT);
  gtk_widget_set_sensitive(Pause_button,FALSE);
  
  gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);
  
  gtk_widget_show(main_window);
  
  // Block images...
  blocks_pixmap = gdk_pixmap_create_from_xpm_d(game_area->window,
					       &mask,
					       NULL,
					       (gchar **)blocks_xpm);	
  
  gtk_main ();
  return 0;
}


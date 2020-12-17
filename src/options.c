#include "tetris.h"

#define OPTIONS_FILE "config.ini"

static GtkWidget * settings_dialog;
static GtkWidget * spin_level;
static GtkWidget * spin_noise_level;
static GtkWidget * spin_noise_height;
static GtkWidget * show_block_chk;


/* returns a path that must be freed with g_free) */
char * get_config_dir_file (const char * file)
{
#define EMPTY_STRING ""
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


void options_defaults (void)
{
   options.start_level = 0;
   options.noise_level = 0;
   options.noise_height = 0;
   options.show_next_block = 1;
}


static void options_save (void)
{
   char * options_f = get_config_dir_file (OPTIONS_FILE);
   GKeyFile * kfile;

   kfile = g_key_file_new ();
   g_key_file_set_integer (kfile, "settings", "start_level", options.start_level);
   g_key_file_set_integer (kfile, "settings", "noise_level", options.noise_level);
   g_key_file_set_integer (kfile, "settings", "noise_height", options.noise_height);
   g_key_file_set_integer (kfile, "settings", "show_next_block", options.show_next_block);
   g_key_file_save_to_file (kfile, options_f, NULL);
   g_key_file_free (kfile);

   g_free (options_f);
}


void options_read (void)
{
   char * options_f = get_config_dir_file (OPTIONS_FILE);
   GKeyFile * kfile;

   kfile = g_key_file_new ();
   if (g_key_file_load_from_file (kfile, options_f,
                                  G_KEY_FILE_NONE, NULL))
   {
      options.start_level = g_key_file_get_integer (kfile, "settings", "start_level", NULL);
      options.noise_level = g_key_file_get_integer (kfile, "settings", "noise_level", NULL);
      options.noise_height = g_key_file_get_integer (kfile, "settings", "noise_height", NULL);
      options.show_next_block = g_key_file_get_integer (kfile, "settings", "show_next_block", NULL);
   }
   g_key_file_free (kfile);

   g_free (options_f);
}


static void settings_dialog_response_cb (GtkDialog * dialog,
                                         int response, gpointer user_data)
{
   if (response == GTK_RESPONSE_OK)
   {
      options.start_level = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin_level));
      options.noise_level = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin_noise_level));
      options.noise_height = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin_noise_height));
      options.show_next_block = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (show_block_chk));
      current_level = options.start_level;
      update_game_values ();
      options_save ();
   }

   gtk_widget_destroy (GTK_WIDGET (dialog));
}


void options_show_dialog (void)
{
  GtkWidget * frame;
  GtkWidget * vbox, * checkbox, * button;
  GtkWidget * vbox_table;
  GtkWidget * hbox_row[3], * labels[3], * spins[3];
  GtkAdjustment * adjs[3];
  int i;

  settings_dialog = gtk_dialog_new ();

  gtk_window_set_title(GTK_WINDOW(settings_dialog),"Settings");
  gtk_window_set_transient_for (GTK_WINDOW (settings_dialog), GTK_WINDOW (main_window));
  gtk_window_set_position (GTK_WINDOW (settings_dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (settings_dialog), TRUE);
  gtk_window_set_resizable (GTK_WINDOW (settings_dialog), FALSE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (settings_dialog), TRUE);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (settings_dialog), TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (settings_dialog), 4);

  vbox = gtk_dialog_get_content_area (GTK_DIALOG (settings_dialog));
  gtk_box_set_spacing (GTK_BOX (vbox), 2);

  // vbox -> frame
  frame = gtk_frame_new ("Blocks");
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
  checkbox = gtk_check_button_new_with_mnemonic ("_Show next block");
  gtk_container_add (GTK_CONTAINER (frame), checkbox);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbox), options.show_next_block);
  gtk_container_set_border_width (GTK_CONTAINER (checkbox),5);
  show_block_chk = checkbox;

  // vbox -> frame
  frame = gtk_frame_new ("Level");
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 2);

  // vbox -> frame -> vbox
  vbox_table = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (frame), vbox_table);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_table), 5);

  labels[0] = gtk_label_new ("Start level: ");
  adjs[0]   = (GtkAdjustment *) gtk_adjustment_new (options.start_level, 0,
                                   NUM_LEVELS-1, 1, 1, 0);
  spins[0]  = gtk_spin_button_new (adjs[0], 0, 0);
  spin_level = spins[0];

  labels[1] = gtk_label_new ("Noise level: ");
  adjs[1]   = (GtkAdjustment *) gtk_adjustment_new (options.noise_level, 0 ,
                                   MAX_X-1, 1, 1, 0);
  spins[1]  = gtk_spin_button_new (adjs[1], 0, 0);
  spin_noise_level = spins[1];

  labels[2] = gtk_label_new ("Noise height: ");
  adjs[2]   = (GtkAdjustment *) gtk_adjustment_new (options.noise_height, 0,
                                   MAX_Y-4, 1, 1, 0);
  spins[2]  = gtk_spin_button_new (adjs[2], 0, 0);
  spin_noise_height = spins[2];

  // vbox -> frame -> vbox -> hbox[x]
  for (i = 0; i < 3; i++)
  {
    hbox_row[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start (GTK_BOX (vbox_table), hbox_row[i], FALSE, FALSE, 2);

    gtk_box_pack_end (GTK_BOX (hbox_row[i]), spins[i],  FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox_row[i]), labels[i], FALSE, FALSE, 0);
  }

  button = gtk_dialog_add_button (GTK_DIALOG (settings_dialog),
                                  "gtk-ok", GTK_RESPONSE_OK);
  button = gtk_dialog_add_button (GTK_DIALOG (settings_dialog),
                                  "gtk-cancel", GTK_RESPONSE_CANCEL);
  gtk_widget_grab_focus (button);

  g_signal_connect (settings_dialog, "response",
                    G_CALLBACK (settings_dialog_response_cb), NULL);

  gtk_widget_show_all (settings_dialog);
}


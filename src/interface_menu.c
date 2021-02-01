
GtkWidget * menuitem_game_start;
GtkWidget * menuitem_game_stop;
GtkWidget * menuitem_game_pause;
GtkWidget * menuitem_game_settings;


static void menu_start (void)
{
   gtk_widget_set_sensitive (menuitem_game_start,    FALSE);
   gtk_widget_set_sensitive (menuitem_game_stop,     TRUE );
   gtk_widget_set_sensitive (menuitem_game_pause,    TRUE );
   gtk_widget_set_sensitive (menuitem_game_settings, FALSE);
}

static void menu_stop (void)
{
   /* game_over_init() */
   gtk_widget_set_sensitive (menuitem_game_start,    TRUE );
   gtk_widget_set_sensitive (menuitem_game_stop,     FALSE);
   gtk_widget_set_sensitive (menuitem_game_pause,    FALSE);
   gtk_widget_set_sensitive (menuitem_game_settings, TRUE );
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem_game_pause), FALSE);
}


static void menu_set_pause (void)
{
   if (game_pause) { /* tetris.h */
      gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem_game_pause), FALSE);
   } else {
      gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem_game_pause), TRUE);
   }
}


static void on_menuitem_start_stop_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   game_start_stop ();
}

static void on_menuitem_pause_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   /* this is also triggered by menu_set_pause()
    *  --> gtk_check_menu_item_set_active()
    * so buttons MUST call menu_set_pause() */
   game_set_pause ();
}

static void on_menuitem_settings_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   options_show_dialog ();
}

static void on_menuitem_quit_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   gtk_widget_destroy (GTK_WIDGET (main_window));
}

static void on_menuitem_help_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   show_help ();
}

static void on_menuitem_scores_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   show_highscore_dlg ();
}

static void on_menuitem_about_activate_cb (GtkMenuItem * menuitem, gpointer user_data) {
   show_about ();
}


static void create_menu_bar (GtkBox * box, GtkWindow * window)
{
   GtkWidget * menu_bar;
   GtkWidget * menu_game;
   GtkWidget * menu_game_header;
   GtkWidget * menuitem_game_quit;
   GtkWidget * menu_help;
   GtkWidget * menu_help_header;
   GtkWidget * menuitem_help_help;
   GtkWidget * menuitem_help_scores;
   GtkWidget * menuitem_help_about;
   GtkWidget * separator_menuitem;
   GtkAccelGroup* accel_group;

   menu_bar = gtk_menu_bar_new ();
   gtk_box_pack_start (GTK_BOX (box), menu_bar, FALSE, FALSE, 0);

   // Game menu
   menu_game = gtk_menu_new ();
   menu_game_header = gtk_menu_item_new_with_mnemonic ("_Game");
   gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_game_header), menu_game);
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_game_header);

   menuitem_game_start = gtk_menu_item_new_with_mnemonic ("Start Game");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), menuitem_game_start);

   menuitem_game_stop = gtk_menu_item_new_with_mnemonic ("Stop Game");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), menuitem_game_stop);

   menuitem_game_pause = gtk_check_menu_item_new_with_mnemonic ("Pause");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), menuitem_game_pause);

   separator_menuitem = gtk_menu_item_new ();
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), separator_menuitem);

   menuitem_game_settings = gtk_menu_item_new_with_mnemonic ("Settings...");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), menuitem_game_settings);

   separator_menuitem = gtk_menu_item_new ();
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), separator_menuitem);

   menuitem_game_quit = gtk_menu_item_new_with_mnemonic ("Quit");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_game), menuitem_game_quit);

   // Help menu
   menu_help = gtk_menu_new();
   menu_help_header = gtk_menu_item_new_with_mnemonic ("_Help");
   gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_help_header), menu_help);
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_help_header);

   menuitem_help_help = gtk_menu_item_new_with_mnemonic ("_Help");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), menuitem_help_help);

   separator_menuitem = gtk_menu_item_new ();
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), separator_menuitem);

   menuitem_help_scores = gtk_menu_item_new_with_mnemonic ("_Top scores");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), menuitem_help_scores);

   separator_menuitem = gtk_menu_item_new ();
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), separator_menuitem);
  
   menuitem_help_about = gtk_menu_item_new_with_mnemonic ("_About");
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), menuitem_help_about);

   // signals - Game Menu
   g_signal_connect (G_OBJECT (menuitem_game_start), "activate",
                     G_CALLBACK (on_menuitem_start_stop_activate_cb), NULL);
   g_signal_connect (G_OBJECT (menuitem_game_stop), "activate",
                     G_CALLBACK (on_menuitem_start_stop_activate_cb), NULL);
   g_signal_connect (G_OBJECT (menuitem_game_pause), "activate",
                     G_CALLBACK (on_menuitem_pause_activate_cb), NULL);
   g_signal_connect (G_OBJECT (menuitem_game_settings), "activate",
                     G_CALLBACK (on_menuitem_settings_activate_cb), NULL);
   g_signal_connect (G_OBJECT (menuitem_game_quit), "activate",
                     G_CALLBACK (on_menuitem_quit_activate_cb), NULL);

   // signals - Help Menu
   g_signal_connect (menuitem_help_help, "activate",
                     G_CALLBACK (on_menuitem_help_activate_cb), NULL);
   g_signal_connect (menuitem_help_about, "activate",
                     G_CALLBACK (on_menuitem_about_activate_cb), NULL);
   g_signal_connect (menuitem_help_scores, "activate",
                     G_CALLBACK (on_menuitem_scores_activate_cb), NULL);

   // accels
   accel_group = gtk_accel_group_new ();
   gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

   gtk_widget_add_accelerator (menuitem_game_start, "activate", accel_group,
                               GDK_KEY_G, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   gtk_widget_add_accelerator (menuitem_game_stop, "activate", accel_group,
                               GDK_KEY_O, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   gtk_widget_add_accelerator (menuitem_game_pause, "activate", accel_group,
                               GDK_KEY_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   gtk_widget_add_accelerator (menuitem_game_settings, "activate",  accel_group,
                               GDK_KEY_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   gtk_widget_add_accelerator (menuitem_game_quit, "activate", accel_group,
                               GDK_KEY_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   gtk_widget_add_accelerator (menuitem_help_help, "activate",  accel_group,
                               GDK_KEY_F1, (GdkModifierType) 0, GTK_ACCEL_VISIBLE);

   menu_stop ();
}


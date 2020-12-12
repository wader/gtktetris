#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "tetris.h"

struct item
{
  char name[10];
  long score;
  int level;
  int lines;
};

struct item highscore[NUM_HIGHSCORE];

void read_highscore()
{
  FILE *fp;
  char * hfile = get_config_dir_file ("highscore.dat");
  if ((fp = fopen (hfile,"r")))
    {
      fread(&highscore,1,sizeof(highscore),fp);
      fclose(fp);
    }
  g_free (hfile);
}

void write_highscore()
{
  FILE *fp;
  char * hfile = get_config_dir_file ("highscore.dat");
  if ((fp = fopen (hfile,"w"))) {
     fwrite(&highscore,1,sizeof(highscore),fp);
     fclose(fp);
  }
  g_free (hfile);
}


void show_highscore(int place)
{
	GtkWidget *highscore_border;
	GtkWidget * dialog;
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *vbox, * button;
	int temp;
	char dummy[40];

	dialog = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (dialog), "Highscores");
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (main_window));
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_window_set_skip_pager_hint (GTK_WINDOW (dialog), TRUE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dialog), TRUE);
	gtk_container_set_border_width (GTK_CONTAINER (dialog), 3);

	vbox = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	highscore_border = gtk_frame_new (NULL);
	gtk_widget_show (highscore_border);
	gtk_box_pack_start (GTK_BOX (vbox), highscore_border, TRUE, TRUE, 5);

	table = gtk_table_new(NUM_HIGHSCORE+1,5,FALSE);
	gtk_container_add (GTK_CONTAINER (highscore_border),table);

	label = gtk_label_new(" # ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
	gtk_widget_set_size_request(label,25,0);
	
	label = gtk_label_new(" Name: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,0,1);
	gtk_widget_set_size_request(label,100,0);
	
	label = gtk_label_new(" Lines: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,2,3,0,1);
	
	label = gtk_label_new(" Level: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,3,4,0,1);
	
	label = gtk_label_new(" Score: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,4,5,0,1);
	
	for(temp=0;temp < NUM_HIGHSCORE;temp++)
	{
		sprintf(dummy,"%d",temp+1);
		label = gtk_label_new(dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);	
		
		sprintf(dummy,"%s",highscore[temp].name);
      		label = gtk_label_new(dummy);

		if(place && place-1 == temp)
		  set_label_with_color (label, "blue", dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);

		sprintf(dummy,"%d",highscore[temp].lines);
		label = gtk_label_new(dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,2,3,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);

		sprintf(dummy,"%d",highscore[temp].level);
		label = gtk_label_new(dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,3,4,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),0.5,0);

		sprintf(dummy,"%lu",highscore[temp].score);
		label = gtk_label_new(dummy);
		if(place && place-1 == temp)
		  set_label_with_color (label, "red", dummy);
		gtk_table_attach_defaults(GTK_TABLE(table),label,4,5,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),1,0);
	}

	button = gtk_dialog_add_button (GTK_DIALOG (dialog), "gtk-close", GTK_RESPONSE_CLOSE);
	gtk_widget_grab_focus (button);
	
	g_signal_connect_swapped (dialog, "response",
	                          G_CALLBACK (gtk_widget_destroy),
	                          (gpointer) dialog);

	gtk_widget_show_all(dialog);
}

int addto_highscore(char *name,long score, int level, int lines)
{
	int place = 0;
	int temp,namelen;
	int added = FALSE;
	
	for(temp=NUM_HIGHSCORE-1;temp > -1;temp--)
	{
		if(score > highscore[temp].score || (highscore[temp].score == 0 && strlen(highscore[temp].name) == 0))
		{
			place = temp;
			added = TRUE;
		}
	}
	if(added)
	{
		for(temp=NUM_HIGHSCORE-1;temp > place;temp--)
			memcpy(&highscore[temp],&highscore[temp-1],sizeof(highscore[0]));
		namelen = strlen(name);
		if(namelen > 9)
			namelen = 9;
		memset(&highscore[place].name,0,sizeof(highscore[0].name));
		memcpy(&highscore[place].name,name,namelen);
		highscore[place].score = score;
		highscore[place].level = level;
		highscore[place].lines = lines;
	}
	return place+1;
}

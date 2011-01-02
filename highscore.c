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

GtkWidget *highscore_window;

struct item highscore[NUM_HIGHSCORE];

void read_highscore()
{
  FILE *fp;
  if((fp = fopen(HIGHSCORE_FILE,"r")))
    {
      fread(&highscore,1,sizeof(highscore),fp);
      fclose(fp);
    }
}

void write_highscore()
{
  FILE *fp;
  if(!(fp = fopen(HIGHSCORE_FILE,"w")))
    return;
  fwrite(&highscore,1,sizeof(highscore),fp);
  fclose(fp);
}

void highscore_close(){
  gtk_widget_hide(highscore_window);}

void show_highscore(int place)
{
	GtkWidget *highscore_border;
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *vbox;
	GtkWidget *Highscore_close_button;
	int temp;
	char dummy[40];
	
	highscore_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(highscore_window),"Highscores");
	gtk_window_set_policy(GTK_WINDOW(highscore_window),FALSE,FALSE,TRUE);
	gtk_window_set_position(GTK_WINDOW(highscore_window),GTK_WIN_POS_NONE/*CENTER*/);
	
	highscore_border = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(highscore_border),GTK_SHADOW_IN/*OUT*/);
	gtk_widget_show(highscore_border);
	gtk_container_add(GTK_CONTAINER(highscore_window),highscore_border);

	vbox = gtk_vbox_new(FALSE,3);
	gtk_container_add(GTK_CONTAINER(highscore_border),vbox);

	table = gtk_table_new(NUM_HIGHSCORE+1,5,FALSE);
	gtk_container_add(GTK_CONTAINER(vbox),table);

	label = gtk_label_new(" # ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
	gtk_widget_set_usize(label,25,0);
	
	label = gtk_label_new(" Name: ");
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(table),label,1,2,0,1);
	gtk_widget_set_usize(label,100,0);
	
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
		  set_gtk_color_style(label,0,0,0xffff);
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
		  set_gtk_color_style(label,0xffff,0,0);
		gtk_table_attach_defaults(GTK_TABLE(table),label,4,5,temp+1,temp+2);
		gtk_misc_set_alignment(GTK_MISC(label),1,0);
	}
	
	Highscore_close_button = gtk_button_new_with_label("Close");	
	gtk_signal_connect(GTK_OBJECT(Highscore_close_button), "clicked",
			   GTK_SIGNAL_FUNC(highscore_close), NULL);	
	gtk_box_pack_start(GTK_BOX(vbox),Highscore_close_button,FALSE,TRUE,0);
  	GTK_WIDGET_SET_FLAGS(Highscore_close_button, GTK_CAN_DEFAULT);
    	gtk_widget_grab_default(Highscore_close_button);

	gtk_widget_show_all(highscore_window);
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

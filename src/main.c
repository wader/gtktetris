/*
 *  main
 * 
 **/

#include "tetris.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main (int argc, char *argv[])
{
  // make sure config dir exists
  char * config_dir = get_config_dir_file (NULL);
  if (access (config_dir, F_OK) != 0) {
     mkdir (config_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  g_free (config_dir);

  // Initialize gtk
  gtk_init (&argc,&argv);

  // init game options
  options_defaults ();
  options_read ();

  create_main_window ();

  // main loop
  gtk_main ();

  return (0);
}

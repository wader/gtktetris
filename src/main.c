/*
 *  main
 * 
 **/

#include "tetris.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#define P_DIR "games"
char * get_config_dir_file (const char * file)
{
   /* returns a path that must be freed with g_free */
   char * config_home = NULL;
   char * res = NULL;
   int freestr = 0;
#if __MINGW32__
   config_home = getenv ("LOCALAPPDATA"); /* XP */
   if (!config_home) {
      config_home = getenv ("APPDATA");
   }
#else
   const char * HOME = NULL;
   config_home = getenv ("XDG_CONFIG_HOME");
   if (!config_home) {
      HOME = getenv ("HOME");
      if (!HOME) {
         HOME = g_get_home_dir ();
      }
      config_home = g_strconcat (HOME, "/.config", NULL);
      freestr = 1;
   }
#endif
   if (file) {
      res = g_strconcat (config_home, G_DIR_SEPARATOR_S, P_DIR,
                         G_DIR_SEPARATOR_S, file, NULL);
   } else {
      res = g_strconcat (config_home, G_DIR_SEPARATOR_S, P_DIR, NULL);
   }
   if (freestr) {
      free (config_home);
   }
   // printf ("%s\n", res); // debug
   return (res);
}


static void ensure_config_dir_exists (void)
{
   char * str = get_config_dir_file ("dummy.file");
   char * p = str ? str+1 : NULL;
   for (; *p; p++)
   {
      if (*p == G_DIR_SEPARATOR) {
         *p = 0;
         if (access (str, F_OK) != 0) {
            if (mkdir (str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
               fprintf (stderr, "%s: Unable to create %s\n", g_get_prgname(), str);
            }
         }
         *p = G_DIR_SEPARATOR;
      }
   }
   g_free (str);
}



int main (int argc, char *argv[])
{
  // Initialize gtk
  gtk_init (&argc,&argv);

  // make sure config dir exists
  ensure_config_dir_exists ();

  // init game options
  options_defaults ();
  options_read ();

  create_main_window ();

  // main loop
  gtk_main ();

  return (0);
}

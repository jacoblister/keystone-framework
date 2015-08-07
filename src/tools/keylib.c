/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "configparser.h"

int main(int argc, char **argv) {
   char config_name[100];
   char buffer[4000];
   int i;

   if (argc < 2) {
      printf("Keystone Application Framework Library tool\n");
      printf("Copyright (C) Jacob Lister, Abbey Systems 2003.  All rights reserved.\n\n");
      printf("usage: keylib <library name> <options>\n");
      exit(1);
      }

   CONFIG_new();
   sprintf(config_name, CONFIG_make_filename(argv[0], CONFIG_FILE_NAME));
   if (!CONFIG_read_file(config_name)) {
      printf("cannot open config file '%s'\n", config_name);
      exit(1);
   };
   if (!CONFIG_key_get(ENV_DIRECTORY)) {
      printf("'%s' environment variable not set\n", ENV_DIRECTORY);
      exit(1);
   }
   if (!CONFIG_key_get(ENV_TARGET)) {
      printf("'%s' environment variable not set\n", ENV_TARGET);
      exit(1);
   }
   if (!CONFIG_key_get("LIB")) {
      printf("LIB (Library tool) not set in config");
      exit(1);
   }

   sprintf(buffer, "%s", CONFIG_key_get("LIB"));

   /*>>>hack!*/
   if (buffer[strlen(buffer) - 1] != ':') {
      sprintf(buffer + strlen(buffer), " ");
   }

   for (i = 1; i < argc; i++) {
      sprintf(buffer + strlen(buffer), "%s ", argv[i]);
   }

   if (system(buffer)) {
      printf("keylib: error running\n");
      exit(1);
   }

   CONFIG_delete();

   return 0;
}

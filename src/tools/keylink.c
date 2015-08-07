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
   char config_name[200];
   char libdir_name[200];
   static char buffer[8000], link_buffer[4000];
   int i;
   int opt_noconsole = 0, opt_gtk = 0;
   int have_resource = 0;
   const char *link_option = "LINK_OPTION";
   const char *keystone_lib = "keystone";

   if (argc < 2) {
      printf("Keystone Application Framework Link tool\n");
      printf("Copyright (C) Jacob Lister, Abbey Systems 2003.  All rights reserved.\n\n");
      printf("usage: keylink <-oappname> <modules>\n");
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

   sprintf(libdir_name, "%s/lib/%s",
           CONFIG_key_get(ENV_DIRECTORY), CONFIG_key_get(ENV_TARGET));
   sprintf(config_name, "%s/bin/%s/%s",
           CONFIG_key_get(ENV_DIRECTORY), CONFIG_key_get(ENV_TARGET), CONFIG_FILE_NAME);
   if (!CONFIG_read_file(config_name)) {
      printf("cannot open config file '%s'\n", config_name);
      exit(1);
   };

   if (!CONFIG_key_get("LINK")) {
      printf("LINK (Link tool) not set in config");
      exit(1);
   }
   if (!CONFIG_key_get("EXEOUT")) {
      printf("EXEOUT environment variable not set");
      exit(1);
   }
   if (!CONFIG_key_get("SUFFIX_OBJ")) {
      printf("SUFFIX_OBJ environment variable not set");
      exit(1);
   }
   if (!CONFIG_key_get("SUFFIX_LIB")) {
      printf("SUFFIX_LIB environment variable not set");
      exit(1);
   }

   sprintf(buffer, "%s ", CONFIG_key_get("LINK"));
   if (argv[1][0] != '-' || argv[1][1] != 'o') {
      printf("first argument no -o output file");
      exit(1);
   }
   sprintf(buffer + strlen(buffer), "%s%s ", CONFIG_key_get("EXEOUT"), &argv[1][2]);
   sprintf(buffer + strlen(buffer), "%s/frame_begin.%s ",
           libdir_name, CONFIG_key_get("SUFFIX_OBJ"));

   for (i = 2; i < argc; i++) {
      if (strlen(argv[i]) >= 4 && (strcmp(&argv[i][strlen(argv[i]) - 4], ".res") == 0 ||
                                    strcmp(&argv[i][strlen(argv[i]) - 4], ".RES") == 0)) {
         have_resource = 1;
      }
      if (argv[i][0] == '-' && argv[i][1] == '-') {
         if (strcmp(argv[i], "--noconsole") == 0) {
             link_option = "LINK_OPTION_NOCONSOLE";
             opt_noconsole = 1;
         }
         else if (strcmp(argv[i], "--gtk") == 0) {
             keystone_lib = "keystone_gtk";
             opt_gtk = 1;
         }
      }
      else {
         sprintf(buffer + strlen(buffer), "%s ", argv[i]);
      }
   }

   if (strcmp(CONFIG_key_get(ENV_TARGET), "win32") == 0 && !have_resource) {
      /*>>>minor kludge */
      sprintf(buffer + strlen(buffer), "%s/frame_end.%s %s/keystone.res %s/%s.%s",
               libdir_name, CONFIG_key_get("SUFFIX_OBJ"),
               libdir_name,
               libdir_name, keystone_lib, CONFIG_key_get("SUFFIX_LIB"));
      }
   else {
      sprintf(buffer + strlen(buffer), "%s/frame_end.%s %s/%s.%s",
              libdir_name, CONFIG_key_get("SUFFIX_OBJ"),
              libdir_name, keystone_lib, CONFIG_key_get("SUFFIX_LIB"));
   }

   if (opt_gtk) {
       /*>>>what a mess, hack for GTK on win32!!!*/
       if (CONFIG_key_get("LINK_PREFIX")) {
          sprintf(link_buffer, "printf \"%s \" >link.rsp", CONFIG_key_get("LINK_PREFIX"));
          system(link_buffer);           
       }
       if (CONFIG_key_get("LINK_GTK_RSP")) {
          sprintf(link_buffer, "%s >gtk.rsp", CONFIG_key_get("LINK_GTK_RSP"));
          system(link_buffer);
       }
       if (CONFIG_key_get(link_option)) {
          sprintf(link_buffer, "printf \"%s \" >option.rsp", CONFIG_key_get(link_option));
          system(link_buffer);
       }
       sprintf(link_buffer, "copy link.rsp+option.rsp+gtk.rsp response.rsp");
       system(link_buffer);

       sprintf(buffer + strlen(buffer), " @response.rsp");
   }
   else {
      if (CONFIG_key_get("LINK_PREFIX")) {
         sprintf(buffer + strlen(buffer), " %s",
                 CONFIG_key_get("LINK_PREFIX"));
      }
      if (CONFIG_key_get(link_option)) {
         sprintf(buffer + strlen(buffer), " %s",
                 CONFIG_key_get(link_option)); 
      }
  }

   printf("exec: %s\n", buffer);
   if (system(buffer)) {
      printf("keylink: error running\n");
      exit(1);
   }

   if (opt_gtk) {
       remove("link.rsp");
       remove("option.rsp");       
       remove("gtk.rsp");              
       remove("response.rsp");              
   }
   
   CONFIG_delete();

   return 0;
}

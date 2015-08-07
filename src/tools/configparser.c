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

#define LINE_BUF_SIZE 250

typedef struct {
   char name[80];
   char value[250];
} TConfigKey;

typedef struct {
   int keycount;
   TConfigKey key[100];
} TConfig;

TConfig config;

void CONFIG_new(void) {
};

void CONFIG_delete(void) {
};

static void CONFIG_parse_line(char *line) {
   char *delimit;
   char *name, *name_end;
   char *value;

   if (*line == '#')
      return;

   delimit = strchr(line, '=');
   if (!delimit)
      return;

   name = line;
   name_end = strchr(name, ' ');
   *name_end = '\0';

   value = delimit + 1;
   while (*value == ' ')
      value++;
   if (value[strlen(value) - 1] == '\n')
      value[strlen(value) - 1] = '\0';

   strcpy(config.key[config.keycount].name, name);
   strcpy(config.key[config.keycount].value, value);
   config.keycount++;
};

char *CONFIG_make_filename(const char *path, const char *filename) {
   static char result[256];
   char *cp;
   sprintf(result, "%s", path);
   cp = strrchr(result, '/');
   if (!cp) {
      cp = strrchr(result, '\\');
   }
   cp[1] = '\0';
   sprintf(result + strlen(result), "%s", filename);
   return result;
};

int CONFIG_read_file(const char *filename) {
   FILE *file;
   char line_buffer[LINE_BUF_SIZE];

   if (!(file = fopen(filename, "r"))) {
      return 0;
   }

   while (fgets(line_buffer, LINE_BUF_SIZE, file)) {
      while (line_buffer[strlen(line_buffer) - 1] == 10 || line_buffer[strlen(line_buffer) - 1] == 13) {
         line_buffer[strlen(line_buffer) - 1] = '\0';
      }
      CONFIG_parse_line(line_buffer);
   }

   fclose(file);

   return 1;
};

char *CONFIG_key_get(const char *keyname) {
   int i;

   for (i = 0; i < config.keycount; i++) {
      if (strcmp(keyname, config.key[i].name) == 0)
         return config.key[i].value;
   }

   return 0;
};

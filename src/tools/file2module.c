/*>>>quick and nasty, files should appear as automatic objects to the
      application in future */

#include <stdio.h>

#define BYTES_PER_LINE 16

int write_file(FILE *dest, const char *input_file_name, const char *symbol_name) {
   FILE *file;
   char buf[BYTES_PER_LINE];
   int chars_read;
   int i, eof, count = 0;

   file = fopen(input_file_name, "rb");

   fprintf(dest, "unsigned char %s[] = {\n", symbol_name);
   do {
      chars_read = fread(buf, 1, BYTES_PER_LINE, file);
      eof = feof(file);
      fprintf(dest, "   ");
      for (i = 0; i < chars_read; i++) {
         count++;
         fprintf(dest, "0x%02x", buf[i]);
         if (i < chars_read - 1 || !eof) {
            fprintf(dest, ",");
         }
      }
      fprintf(dest, "\n");
   } while (!eof);

   fprintf(dest, "};\n");

   fclose(file);

   return count;
}/*write_file*/

int main(int argc, char **argv) {
   FILE *file;
   int i, count;
   char data_name[80], *cp;

   if (argc < 3) {
      printf("Keystone Application Framework File to Module tool\n");
      printf("Copyright (C) Jacob Lister, Abbey Systems 2003.  All rights reserved.\n\n");
      printf("usage: file2module <outfile> <infiles>\n");
      return 1;
   }

   file = fopen(argv[1], "w");
   fprintf(file, "MODULE::IMPLEMENTATION\n");
   fprintf(file, "#include \"framework.c\"\n");


   for (i = 0; i < argc - 2; i++) {
      sprintf(data_name, "_file_%s", argv[2 + i]);
      cp = data_name;
      while (*cp) {
         if (*cp == '.')
            *cp = '_';
         cp++;
      }

      count = write_file(file, argv[2 + i], data_name);
      fprintf(file, "TMemFileDef _%s_def = {\"%s\", %d, %s};\n", data_name, argv[2 + i], count, data_name);
      fprintf(file, "OBJECT<CMemFile, _%s, &_%s_def>;\n\n", data_name, data_name);
   }
   fprintf(file, "MODULE::END\n");

   fclose(file);

   return 0;
}

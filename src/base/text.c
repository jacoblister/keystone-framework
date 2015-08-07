/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include "environ.h"
#include "text.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Global int ENUM_decode(int count, const char **element, const char *string) {
   int i;

   for (i = 0; i < count; i++) {
      if (strcmp(string, element[i]) == 0) {
         return i;
      }
   }

   return -1;
}/*ENUM_decode*/

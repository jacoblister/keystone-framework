#ifndef I_TEXT
#define I_TEXT

#include "environ.h"

typedef struct {
   int count;
   const char **name;
} TEnum;

int ENUM_decode(int count, const char **element_name, const char *string);

#endif/*I_TEXT*/

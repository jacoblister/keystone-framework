/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#ifndef I_ENV
#define I_ENV

#define DEBUG TRUE      /*>>>to makefile*/

/*>>>think about moving these out of here, system specific*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifdef OS_Linux
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#endif

#if COMP_MSVC
#define inline __inline
typedef unsigned long ulong;
#endif

typedef unsigned char byte;
typedef unsigned short word;
typedef int bool;

#ifndef TRUE
#define FALSE 0
#define TRUE !FALSE
#endif

#ifndef NULL
#define NULL            0L
#endif

#define Global
#define Local           static

#define CLEAR(x) memset(x, 0, sizeof(*x))
#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

#define strsize(x) (strlen(x) + 1)

#define SID_INIT              0xF0F1F3F4
#if DEBUG
#define SID_OBJ               0xE0E1E3E4
#define SID_OBJ_SPECIAL       0xE0E1E3E5
#define SID_OBJ_ARRAY_STATIC  0xE0E1E3E6
#define SID_OBJ_ARRAY_DYNAMIC 0xE0E1E3E7
#define SID_OBJ_DELETED       0xE4E3E1E0
#endif

#define PI 3.14159265

/* Assertion Mechanism */
#ifdef OS_Linux
extern pid_t main_proc_id;
#endif

#define ABORT() {int exit = 0; exit = 1 / exit;}
#define ASSERT(str) {fprintf(stderr, "EXIT:%s\n", str); ABORT();}
//#define WARN(str) {fprintf(stderr, "WARNING:%s\n", str);}
#define WARN(str) {printf("WARNING:%s\n", str);}

#endif

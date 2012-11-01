/*
 * Fake stdlib.h for old systems
 */

#include <sys/types.h>
#ifndef NULL
#define NULL (char *)0
#endif
char *getenv();
char *malloc();
char *calloc();
char *realloc();
long atol();
extern int errno;

#include <config.h>
#include "proto.h"
#include <stdio.h>
#include <ctype.h>
#include <useconfig.h>
#include "dict.h"
#include "phones.h"
#include "getargs.h"

static void show PROTO((char *s));
static void 
show(s)
char *s;
{
 unsigned char *p = dict_find(s, strlen(s));
 printf("%s", s);
 if (p)
  {
   int l = strlen((char *)p);
   int i;
   for (i = 0; i < l; i++)
    printf(" %s", ph_name[(unsigned) (p[i])]);
   printf(" [");
   for (i = 0; i < l; i++)
    printf("%s", dialect[(unsigned) (p[i])]);
   printf("]\n");
   free(p);
  }
 else
  printf(" ???\n");
}

int main PROTO((int argc, char *argv[], char *env[]));

int
main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{
 argc = dict_init(argc, argv);
 if (help_only)
  {
   fprintf(stderr,"Usage: %s [options as above] words to lookup\n",argv[0]);
  }
 else
  {
   if (dict)        
    {               
     int i;
     for (i=1; i < argc; i++)
      {
       show(argv[i]);
      }
     dict_term();   
    }               
  }
 return 0;
}

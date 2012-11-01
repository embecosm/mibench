#include <config.h>
#include "proto.h"
#include <stdio.h>
#include <ctype.h>
#include <useconfig.h>
#include <gdbm.h>
#include "trie.h"
#include "darray.h"
#include "phones.h"

/* Don't force block size let gdbm/os decide */
#define BLOCK_SIZE 0

#ifndef GDBM_FAST
/* Tolerate older versions of gdbm ... */
#define GDBM_FAST 0
#endif

trie_ptr phones;

static void enter_phones PROTO((void));
static void 
enter_phones()
{
 int i;
 char *s;
 for (i = 1; (s = ph_name[i]); i++)
  trie_insert(&phones, s, (void *) i);
}

static void enter_words PROTO((GDBM_FILE db, FILE * f));
static void
enter_words(db, f)
GDBM_FILE db;
FILE *f;
{
 char buf[4096];
 while (fgets(buf, sizeof(buf), f))
  {
   char *s = buf;
   char *h = strchr(s, '#');
   if (h)
    *h = '\0';
   while (isspace(*s))
    s++;
   if (*s)
    {
     char *p = s;
     while (isalpha(*p) || *p == '-' || *p == '\'')
      {
       if (islower(*p))
        *p = toupper(*p);
       p++;
      }
     if (isspace(*p))
      {
       char codes[4096];
       char *d = codes;
       int ok = 1;
       datum key;
       key.dptr = s;
       key.dsize = p - s;
       while (*p && ok)
        {
         unsigned code;
         while (isspace(*p))
          p++;
         if (*p)
          {
           char *e = p;
           while (isalpha(*e) || *e == '1' || *e == '2')
            {
             if (islower(*e))
              *e = toupper(*e);
             e++;
            }
           if (*e == '0')
            *e++ = ' ';
           if (e > p && (code = (unsigned) trie_lookup(&phones, &p)))
            *d++ = code;
           else
            {
             fprintf(stderr, "Bad code %.*s>%s", (int)(p - s), s, p);
             ok = 0;
             break;
            }
          }
        }
       if (ok)
        {
         datum data;
         data.dptr = codes;
         data.dsize = d - codes;
         gdbm_store(db, key, data, GDBM_INSERT);
        }
      }
     else
      {
       if (*p != '(')
        fprintf(stderr, "Ignore (%c) %s", *p, s);
      }
    }
  }
}

int main PROTO((int argc, char *argv[], char *env[]));

int
main(argc, argv, envp)
int argc;
char *argv[];
char *envp[];
{
 if (argc == 3)
  {
   FILE *f = fopen(argv[1], "r");
   if (f)
    {
     GDBM_FILE db = gdbm_open(argv[2], BLOCK_SIZE, GDBM_WRCREAT | GDBM_FAST, 0644, NULL);
     if (db)
      {
       enter_phones();
       enter_words(db, f);
       gdbm_close(db);
      }
     else
      perror(argv[2]);
     fclose(f);
    }
   else
    perror(argv[1]);
  }
 return 0;
}

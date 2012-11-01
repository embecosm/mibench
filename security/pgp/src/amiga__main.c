/*
$VER: RKNOP's __main() 1.2 (06.02.95)

  Replacement AmigaDOS __main() for SAS/C 6.51.

  As with the standard __main(), this parses the input command line into the
  argv array.  However, here, the number of arguments is unlimited;
  additionally, one may specify on the command line a list of arguments in
  a file using the construction "@filename" (without quotes).  The named file
  will be read and all of the whitespace separated arguments therein will be
  inserted to the argv array at that point.  Since the string parser is written
  recursively, embedded files may be nested; that is, within one filename
  referenced on the command line with @, there may be another filename
  referenced with @.  Memory and stack space is the only limit as to how far
  the nesting may go.

  Version 1.2 -- this version supports stderr redirection, similar to unix, by
  specifying &filename (or & filename or &"file name" or & "file name") on the
  command line.  stderr is redirected by opening first calling Close() on what
  is currently in __ufbs[2].ufbfh (hopefully this simulates the SAS stdio
  destructor), and then Open()ing the file __ufbs[2].ufbfh with the specified
  filename.

  Inspiration by Peter Simons.  Rewritten from scratch by Robert Knop using no
  SAS code so that this file could be freely distributed with PGP 2.6
  source code.

  rknop@netcom.com
  rknop@citir5.caltech.edu
  R.KNOP1 on GEnie
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ios1.h>
#include <dos/dos.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <workbench/startup.h>

#include <proto/dos.h>
#include <proto/exec.h>

#define isspace(c)   (((c)==' ')||((c)=='\t')||((c)=='\n'))

extern struct WBStartup *_WBenchMsg;

int main(int,void *);
int __regargs CountArgs(struct MinList *,struct ArgNode *);
void __regargs AddArgs(struct ArgNode **,char ***);

struct ArgNode {
   struct MinNode n;
   int length;
   char *d;
};

void __stdargs __main(char *inline)
{  struct ArgNode n,*cur;
   struct MinList ArgStrings;
   int argc,rval;
   char **argv,**pargv;

   NewList((struct List *)&ArgStrings);
   n.d=inline;
   AddTail((struct List *)&ArgStrings,(struct Node *)&n);

   if (inline && (argc=CountArgs(&ArgStrings,&n)))
   {  if (!(argv=(char **)calloc(argc+1,sizeof(char *))))
         /* +1 is for one null element at the end of argv */
         exit(20);
      cur=&n;
      pargv=argv;
      AddArgs(&cur,&pargv);
   }
   else argv=(char **)_WBenchMsg;   /* Workbench startup */

   rval=main(argc,argv);

/* Don't worry about cleanup -- we used only calloc & malloc */
/* (It's safer this way, so that if the program goes directly to exit()
   we don't have to worry about missed cleanup.) */

   exit(rval);
}

/**********************************************************************/
/* CountArgs() -- a recursive function that counts the number of      */
/*                arguments in the string in node.d.  If, while       */
/*                counting, it comes across an argument of the form   */
/*                "@filename", it creates a new node on the tail of   */
/*                list, reads the data from the named file            */
/*                into this new node's data, and recursively calls    */
/*                itself to count the arguments in the new read string*/
/**********************************************************************/

int __regargs CountArgs(struct MinList *list,struct ArgNode *node)
{  int argc;
   struct ArgNode *n;
   char *line,*fstart,fendbuffer;
   BPTR ifp;                         /* Only 1 file will be open at a time */
   static struct FileInfoBlock fib;  /* static safe for recursion */

   argc=0;
   line=node->d;
   while (1)
   {  while (isspace(*line)) line++;   /* Skip leading whitespace */

      if (*line=='\0') break;

      else if (*line=='&')          /* Skip a stderr redirect */
      {  ++line;
         while (isspace(*line)) ++line;
         if (*line=='"')
         {  line++;
            while (*line!='"' && *line!='\0') line++;
            if (*line=='"') line++;
         }
         else
         {  while (!isspace(*line))
            {  if (*line=='\0') break;
               ++line;
            }
         }
      }

      else if (*line=='@')         /* Embedded file */
      {  fstart=++line;
         if (*line=='"')
         {  fstart++; line++;
            while (*line!='"')
              if (*(++line)=='\0') break;  /* Errorish? */
            fendbuffer=*line;
            *line='\0';  /* Temporarily 0-term so we can use as a filename */
         }
         else
         {  while (!isspace(*line))
              if (*(++line)=='\0') break;    /* Error? */
            fendbuffer=*line;
            *line='\0';   /* Temp 0-term for filename */
         }
         if (!(ifp=Open(fstart,MODE_OLDFILE)))
            fprintf(stderr,"Couldn't open file \"%s\"\n",fstart);
         else
         {  ExamineFH(ifp,&fib);
            n=(struct ArgNode *)calloc(sizeof(struct ArgNode),1);
               /* CHECK ERRORS!!!!!!!!!!! */
            AddTail((struct List *)list,(struct Node *)n);
            if (fib.fib_Size)
            {  n->d=calloc((n->length=fib.fib_Size+1),1);
               Read(ifp,n->d,fib.fib_Size);
               n->d[fib.fib_Size]='\0';   /* Make sure null terminated */
            }
            Close(ifp);
            if (n->d) argc+=CountArgs(list,n);          /* Recurse */
         }
         *line=fendbuffer;        /* Restore char replaced with '\0' */
         if (*line=='"') ++line;  /* Increment line past quote */
      }    /* End of embedded file handling */

      else if (*line=='"')
      {  ++line;
         while (*line!='"')
           if (*(++line)=='\0') break;
         argc++;
         if (*line='"') ++line;
      }

      else
      {  while (!isspace(*line))
           if (*(++line)=='\0') break;
         argc++;
      }

   }     /* End of infinite loop */

   return(argc);   /* Also returns any nodes added to list from embed files */
}

/**********************************************************************/
/* AddArgs() -- put each of the arguments in the node into the array  */
/*              argv.  Recurse if necessary                           */
/**********************************************************************/

void __regargs AddArgs(struct ArgNode **node,char ***argv)
{  char *line,*stderrfile;

   line=(*node)->d;
   (*node)=(struct ArgNode*)(*node)->n.mln_Succ;
   while (1)
   {  while (isspace(*line)) line++;   /* Skip leading whitespace */

      if (*line=='\0') break;                  /* Finished */

      else if (*line=='&')                     /* stderr redirect */
      {  ++line;
         while (isspace(*line)) ++line;
         if (*line=='"')
         {  stderrfile=++line;
            while (*line!='"' && *line!='\0') line++;
            if (*line=='"') { *line='\0'; line++; }
         }
         else
         {  stderrfile=line;
            while (!isspace(*line))
            {  if (*line=='\0') break;
               line++;
            }
            if (*line!='\0') { *line='\0'; line++; }
         }
         Close(__ufbs[2].ufbfh);
         if ((__ufbs[2].ufbfh=Open(stderrfile,MODE_NEWFILE))==NULL)
           __ufbs[2].ufbfh=Open("NIL:",MODE_OLDFILE);
      }

      else if (*line=='@')         /* Embedded file -- skip past filename */
      {  ++line;
         if (*line=='"')
         {  while (*line!='"')
              if (*(++line)=='\0') break;
         }
         else
         {  while (!isspace(*line))
              if (*(++line)=='\0') break;
         }
         AddArgs(node,argv);       /* Add the arguments from whatever node */
         if (*line!='\0') ++line;
      }

      else if (*line=='"')
      {  (*argv)[0]=++line;
         while (*line!='"')
           if (*(++line)=='\0') break;
         if (*line!='\0')
         {  *line='\0';                /* Null terminate this argument */
            ++line;
         }
         (*argv)++;
      }

      else
      {  (*argv)[0]=line;
         while (!isspace(*line))
           if (*(++line)=='\0') break;
         if (*line!='\0')
         {  *line='\0';               /* Null terminate this argument */
            ++line;
         }
         (*argv)++;
      }
   }     /* End of infinite loop */
}

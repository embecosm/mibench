 /******************************************************
 *                                                     *
 *     PGPSort: a utility to sort PGP 2.x keyrings     *
 *               .                                     *
 *  (c)1995 by Stale Schumacher <stale@hypnotech.com>  *
 *              Version 1.01 - 1995/09/04              *
 *                                                     *
 *  This program is placed in the public domain and    *
 *  may be freely distributed and modified, as long    *
 *  as this copyright notice remains intact. The code  *
 *  was written from scratch based on the PGP 2.x      *
 *  format specifications, and contains no source      *
 *  from the PGP distribution. Based on a TurboPascal  *
 *  implementation by the same author.                 *
 *                                                     *
 *  DISCLAIMER: PGPSort will always make a backup of   *
 *  the keyring before sorting it, but the author      *
 *  cannot take any responsibility for damage that     *
 *  the program may cause. Use PGPSort at your own     *
 *  risk!                                              *
 *                                                     *
 *  NOTE FOR PROGRAMMERS: This program was written     *
 *  for portability, _not_ for speed! :-)              *
 *                                                     *
 ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXKEYS 20000  /* Increase this if necessary */

#ifndef TRUE
#define FALSE 0
#define TRUE  !FALSE
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef FILENAME_MAX
#include <sys/param.h>
#ifdef MAXPATHLEN
#define FILENAME_MAX MAXPATHLEN
#endif
#endif

typedef unsigned char  byte;
typedef unsigned char  boolean;
typedef unsigned short word16;
typedef unsigned long  word32;

#define CTB_PUBKEY 24
#define CTB_SECKEY 20
#define CTB_USERID 52
#define CTB_TRUST  48
#define CTB_SIG     8

#define ERR_OK                0
#define ERR_ILLEGAL_PARAMS    1
#define ERR_FILE_NOT_OPEN     2
#define ERR_NOT_A_KEYRING     3
#define ERR_COULD_NOT_BACKUP  4
#define ERR_KEYRING_CORRUPT   5
#define ERR_OUT_OF_MEMORY     6

#define SORT_NOSORT     0
#define SORT_ON_USERID  1
#define SORT_ON_KEYID   2
#define SORT_ON_DATE    3
#define SORT_ON_SIZE    4

#define ASCENDING  TRUE
#define DESCENDING FALSE

#define BADKEY_NO_USERID     0
#define BADKEY_BOGUS_USERID  1
#define BADKEY_SECRET_KEY    2

struct keydata {
   long    fpos;
   word32  length,
	   keyid,
	   date;
   word16  size;
   char    userid[26];
   boolean secret,
           removed;
};

struct  keydata *key[MAXKEYS];
int     keys = 0;
int     sort_criterion;
boolean sort_order;
boolean remove_bad_keys;

word32 get_packet_length(byte ctb, FILE *fp)
{
   byte   l[4] = {1,2,4,0},
	  llength = l[ctb & 0x03],
	  i;
   word32 length = 0;

   for (i=0; i<llength; i++)
      length = (length << 8) + (byte) fgetc(fp);
   return length;
}

word32 read_word32(FILE *fp)
{
   int    i;
   word32 word;

   for (i=0; i<32/8; i++)
      word = (word << 8) + (byte) fgetc(fp);
   return word;
}

word16 read_word16(FILE *fp)
{
   int    i;
   word16 word;

   for (i=0; i<16/8; i++)
      word = (word << 8) + (byte) fgetc(fp);
   return word;
}

char *strupper(char *s)
{
   char *p;
   for (p=s; *p; p++)
      *p=toupper(*p);
   return s;
}

char *strend(char *s)
{
   char *p;
   for (p=s; *p; p++);
   return p;
}

char *strstrip(char *s)
{
   char *p;
#  define stripchar(c) (c==' ' || c=='>' || c=='-')

   p=strend(s);
   for (p--; p >= s && stripchar(*p); p--)
      *p='\0';
   return s;
}

char *force_extension(char *filename, char *extension)
{
   char *p;
   p=strrchr(filename, '.');
   if (!p) p=strend(filename);
   strcpy(p, extension);
   return filename;
}

char *add_slash(char *directory)
{
   char *p;
   p=strend(directory); p--;
   if (*p != '/' && strchr(directory, '/'))
      strcat(directory, "/");   /* Unix */
   if (*p != '\\' && strchr(directory, '\\'))
      strcat(directory, "\\");  /* MS-DOS */
   return directory;
}

void prompt_for_removal(int keynr, char *userid, int cause)
{
   char date[11];
   int  c;

   switch (cause) {
      case BADKEY_NO_USERID:
         printf("The following key has no attached userID:\n"); break;
      case BADKEY_BOGUS_USERID:
         printf("The following key seems to have a bogus user ID:\n"); break;
      case BADKEY_SECRET_KEY:
         printf("The keyring contains a secret key:\n"); break;
   }
   if (key[keynr]->secret)
     printf("sec  ");
   else
     printf("pub  ");
   strftime(date,sizeof(date),"%Y/%m/%d",gmtime(&key[keynr]->date));
   printf("%4d/%8lX %s %s\n",key[keynr]->size,key[keynr]->keyid,date,userid);
   printf("Remove it <y/N>? ");
   fflush(stdin); c = getchar();
   key[keynr]->removed = (c=='y' || c=='Y');
   printf("\n");
}

char *get_username(char *userid)
{
   char s[256];
   char *p = userid;

   /* Isolate name from email address, telephone number etc. */
   while (isalpha(*p) || isspace(*p) || *p=='.' || *p=='-') p++;
   if (p > userid) *p='\0';
   strupper(strstrip(userid));

   /* Derive name from email address? */
   if (*userid == '<') {
      strcpy(userid, userid+1);
   }

   /* Remove titles */
   p=strend(userid);
   if (!strcmp(p-2, " I")) *(p-2)='\0';
   if (!strcmp(p-3, " II")) *(p-3)='\0';
   if (!strcmp(p-4, " III")) *(p-4)='\0';
   if (!strcmp(p-4, " JR.")) *(p-4)='\0';
   if (!strcmp(p-5, " M.D.")) *(p-5)='\0';

   /* Swap first and last names */
   if ((p = strrchr(userid, ' ')) != NULL) {
      strcpy(s,p+1);
      *p = '\0';
      strcpy(userid, strcat(s, userid));
   }

   /* Return the user name on a form that may easily be sorted */
   return userid;
}

int keycmp(struct keydata *key1, struct keydata *key2)
{
   int c;
   switch (sort_criterion) {
      case SORT_ON_USERID: c = strcmp(key1->userid, key2->userid) ; break;
      case SORT_ON_KEYID : c = key1->keyid < key2->keyid ? -1 : +1; break;
      case SORT_ON_SIZE  : c = key1->size  < key2->size  ? -1 : +1; break;
      case SORT_ON_DATE  : c = key1->date  < key2->date  ? -1 : +1; break;
   }
   if (sort_order==DESCENDING) c = -c;
   return c;
}

void swap(struct keydata *v[], int i, int j)
{
   struct keydata *temp;
   temp = v[i]; v[i] = v[j]; v[j] = temp;
}

void sort_keydata(struct keydata *v[], int left, int right)
{
   int i, last;
   if (left >= right) return;
   swap(v, left, (left+right)/2);
   last = left;
   for (i = left+1; i <= right; i++)
      if (keycmp(v[i], v[left]) < 0)
	 swap(v, ++last, i);
   swap(v, left, last);
   sort_keydata(v, left, last-1);
   sort_keydata(v, last+1, right);
}

int sort_keyring(char *keyring)
{
   FILE    *ifp,
	   *ofp;
   long    count,
	   fpos = 0L,
	   ctbpos = 0L;
   byte    ctb;
   word32  packet_length;
   char    userid[256],
	   bakring[FILENAME_MAX];
   boolean is_pubring,
           first_userid = FALSE;
   byte    buf[256];
   int     i,
	   status = ERR_OK;

#  define error(s) {status = s; goto end;}

   /* Read through the whole keyring and gather keydata */
   if (!(ifp = fopen(keyring,"rb")))
      return ERR_FILE_NOT_OPEN;
   count = fread(&ctb, 1, 1, ifp);
   if ((ctb & 60) == CTB_PUBKEY)
      is_pubring = TRUE;
   else if ((ctb & 60) == CTB_SECKEY)
      is_pubring = FALSE;
   else
      error(ERR_NOT_A_KEYRING);
   while (count > 0) {
      if (!(ctb & 0x80))
	 error(ERR_KEYRING_CORRUPT);
      packet_length = get_packet_length(ctb, ifp);
      fpos = ftell(ifp);
      ctb = (ctb & 60);
      switch (ctb) {
	 case CTB_PUBKEY:
	 case CTB_SECKEY:
            if (first_userid && remove_bad_keys && (!key[keys-1]->removed))
               prompt_for_removal(keys-1,"",BADKEY_NO_USERID);
	    if (keys == MAXKEYS)
	       error(ERR_OUT_OF_MEMORY);
	    if (!(key[keys] = malloc(sizeof(struct keydata))))
	       error(ERR_OUT_OF_MEMORY);
	    if (keys>0) key[keys-1]->length = ctbpos - key[keys-1]->fpos;
	    key[keys]->fpos = ctbpos;
	    key[keys]->length = packet_length;
	    fseek(ifp, fpos+1, SEEK_SET); key[keys]->date = read_word32(ifp);
	    fseek(ifp, fpos+8, SEEK_SET); key[keys]->size = read_word16(ifp);
	    fseek(ifp, fpos+6+(key[keys]->size+7)/8, SEEK_SET);
	    key[keys]->keyid = read_word32(ifp);
            key[keys]->removed = FALSE;
            key[keys]->secret = (ctb==CTB_SECKEY);
	    first_userid = TRUE;
	    keys++;
	    break;
	 case CTB_USERID:
	    fread(&userid, packet_length, 1, ifp);
	    userid[packet_length]='\0';
            if (remove_bad_keys && (!key[keys-1]->removed)) {
               if (strstr(userid,"FUCK") || strstr(userid,"DICK") ||
                  strstr(userid,"SHIT") || strstr(userid,"BEGIN") ||
                  strstr(userid,"***") || strstr(userid,"@whitehouse.gov") ||
                  strlen(userid) > 150)
                  prompt_for_removal(keys-1,userid,BADKEY_BOGUS_USERID);
	    }
	    if (first_userid) {
               if (key[keys-1]->secret && remove_bad_keys && (!key[keys-1]->removed)
                  && is_pubring)
                  prompt_for_removal(keys-1,userid,BADKEY_SECRET_KEY);
	       strncpy(key[keys-1]->userid, get_username(userid),
		       sizeof(key[keys-1]->userid));
	       first_userid = FALSE;
	    }
	 case CTB_TRUST:
	 case CTB_SIG:
	    break;
	 default:
	    error(ERR_NOT_A_KEYRING);
      }
      fseek(ifp, ctbpos = fpos += packet_length, SEEK_SET);
      count = fread(&ctb, 1, 1, ifp);
   }
   key[keys-1]->length = ctbpos - key[keys-1]->fpos;
   fclose(ifp);

   /* Sort keydata in main memory */
   if (sort_criterion != SORT_NOSORT)
      sort_keydata(key,0,keys-1);

   /* Backup the unsorted keyring in case something goes wrong */
   strcpy(bakring,keyring);
   force_extension(bakring,".bak");
   remove(bakring);
   if (rename(keyring,bakring))
      error(ERR_COULD_NOT_BACKUP);

   /* Copy the keys from old to new keyring in sorted order */
   if (!(ifp=fopen(bakring,"rb")))
      error(ERR_COULD_NOT_BACKUP);
   if (!(ofp=fopen(keyring,"wb")))
      error(ERR_COULD_NOT_BACKUP);
   for (i=0; i<keys; i++)
      if (!key[i]->removed) {
         fseek(ifp,key[i]->fpos, SEEK_SET);
         while (key[i]->length > 0) {
	    size_t bytes = MIN(key[i]->length, sizeof(buf));
	    fread(&buf, bytes, 1, ifp);
	    fwrite(&buf, bytes, 1, ofp);
	    key[i]->length -= bytes;
	 }
      }
   fclose(ofp);

end:
   /* Remember to cleanup after us */
   fclose(ifp);
   for (i = 0; i < keys; i++)
      free(key[i]);
   return status;
}

void show_usage()
{
   fprintf(stderr, "\nPGPSort v1.01  (c) 1995 stale@hypnotech.com\n\n");
   fprintf(stderr, "Synopsis: Sorts PGP 2.x keyrings\n\n");
   fprintf(stderr, "Usage   : pgpsort +u[r] [keyring] - sort on user ID\n");
   fprintf(stderr, "          pgpsort +k[r] [keyring] - sort on key ID\n");
   fprintf(stderr, "          pgpsort +s[r] [keyring] - sort on key size\n");
   fprintf(stderr, "          pgpsort +d[r] [keyring] - sort on date of creation\n");
   fprintf(stderr, "          pgpsort -r    [keyring] - remove keys (no sorting)\n\n");
   fprintf(stderr, "          Use 'r' to remove bad keys\n");
   fprintf(stderr, "          Use '-' instead of '+' to sort in descending order\n\n");
   exit(ERR_ILLEGAL_PARAMS);
}

int main(int argc, char *argv[])
{
   char *sort_option;
   char filename[FILENAME_MAX];

   /* Parse command line options */
   if (argc==2 || argc==3) {
      sort_option = argv[1];
      if (argc==3)
	 strcpy(filename, argv[2]);
      else {
	 printf("No filename given, assuming default keyring.\n");
	 if (getenv("PGPPATH"))
	    add_slash(strcpy(filename, getenv("PGPPATH")));
	 else if (getenv("HOME") && strchr(getenv("HOME"), '/'))
 	    strcat(add_slash(strcpy(filename, getenv("HOME"))), ".pgp/");
         else
	    filename[0]='\0';
	 strcat(filename, "pubring.pgp");
      }
   } else
     show_usage();
   if (strlen(sort_option)==2 || strlen(sort_option)==3) {
      switch (sort_option[0]) {
         case '+': sort_order=ASCENDING;  break;
         case '-': sort_order=DESCENDING; break;
         default : show_usage();
      }
      if (strlen(sort_option)==2)
         remove_bad_keys = FALSE;
      else if (sort_option[1]!='r' && sort_option[2]=='r')
         remove_bad_keys = TRUE;
      else
         show_usage();
      switch (sort_option[1]) {
         case 'u': sort_criterion=SORT_ON_USERID; break;
         case 'k': sort_criterion=SORT_ON_KEYID;  break;
         case 's': sort_criterion=SORT_ON_SIZE;   break;
         case 'd': sort_criterion=SORT_ON_DATE;   break;
         case 'r': sort_criterion=SORT_NOSORT;
                   remove_bad_keys=TRUE;          break;
         default : show_usage();
      }
   } else
      show_usage();

   /* OK, attempt to sort the keyring */
   switch (sort_keyring(filename)) {
      case ERR_OK:
	 printf("Keyring '%s' ", filename);
	 switch (sort_criterion) {
	    case SORT_NOSORT   : printf("processed.\n"); break;
	    case SORT_ON_USERID: printf("sorted on user ID.\n"); break;
	    case SORT_ON_KEYID : printf("sorted on key ID.\n"); break;
	    case SORT_ON_SIZE  : printf("sorted on key size.\n"); break;
	    case SORT_ON_DATE  : printf("sorted on date.\n");
	 }
	 return ERR_OK;
      case ERR_FILE_NOT_OPEN:
	 fprintf(stderr, "Could not open keyring file '%s'.\n", filename);
	 return ERR_FILE_NOT_OPEN;
      case ERR_NOT_A_KEYRING:
	 fprintf(stderr, "File '%s' is not a PGP 2.x keyring.\n", filename);
	 return ERR_NOT_A_KEYRING;
      case ERR_COULD_NOT_BACKUP:
	 fprintf(stderr, "Could not create backup file, keyring not sorted.\n");
	 return ERR_COULD_NOT_BACKUP;
      case ERR_KEYRING_CORRUPT:
	 fprintf(stderr, "Keyring '%s' is corrupt, cannot sort it.\n", filename);
	 return ERR_KEYRING_CORRUPT;
      case ERR_OUT_OF_MEMORY:
	 fprintf(stderr, "Out of memory: keyring '%s' is too big to sort.\n", filename);
	return ERR_OUT_OF_MEMORY;
   }

   return 0;  /* Just to please some compilers */
}

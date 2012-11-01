#ifndef lint
static char Rcs_Id[] =
    "$Id: buildhash.c,v 1.64 1995/01/08 23:23:26 geoff Exp $";
#endif

#define MAIN

/*
 * buildhash.c - make a hash table for okspell
 *
 * Pace Willisson, 1983
 *
 * Copyright 1992, 1993, Geoff Kuenning, Granada Hills, CA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All modifications to the source code must be clearly marked as
 *    such.  Binary redistributions based on modified source code
 *    must be clearly marked as modified versions in the documentation
 *    and/or other materials provided with the distribution.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by Geoff Kuenning and
 *      other unpaid contributors.
 * 5. The name of Geoff Kuenning may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log: buildhash.c,v $
 * Revision 1.64  1995/01/08  23:23:26  geoff
 * Make the various file suffixes configurable for DOS purposes.
 *
 * Revision 1.63  1994/10/26  05:12:25  geoff
 * Get rid of some duplicate declarations.
 *
 * Revision 1.62  1994/07/28  05:11:33  geoff
 * Log message for previous revision: distinguish a zero count from a bad
 * count file.
 *
 * Revision 1.61  1994/07/28  04:53:30  geoff
 *
 * Revision 1.60  1994/01/25  07:11:18  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "msgs.h"
#include "version.h"
#include <ctype.h>
#include <sys/stat.h>

int		main P ((int argc, char * argv[]));
static void	output P ((void));
static void	filltable P ((void));
VOID *		mymalloc P ((unsigned int size));
VOID *		myrealloc P ((VOID * ptr, unsigned int size,
		  unsigned int oldsize));
void		myfree P ((VOID * ptr));
static void	readdict P ((void));
static void	newcount P ((void));

#define NSTAT	100		/* Size probe-statistics table */

struct stat	dstat;		/* Result of stat-ing dict file */
struct stat	cstat;		/* Result of stat-ing count file */

char *		Dfile;		/* Name of dictionary file */
char *		Hfile;		/* Name of hash (output) file */
char *		Lfile;		/* Name of language file */

char		Cfile[MAXPATHLEN]; /* Name of count file */
char		Sfile[MAXPATHLEN]; /* Name of statistics file */

static int silent = 0;		/* NZ to suppress count reports */

int main (argc, argv)
    int		argc;
    char *	argv[];
    {
    int		avg;
    FILE *	countf;
    FILE *	statf;
    int		stats[NSTAT];
    int		i;
    int		j;

    while (argc > 1  &&  *argv[1] == '-')
	{
	argc--;
	argv++;
	switch (argv[0][1])
	    {
	    case 's':
		silent = 1;
		break;
	    }
	}
    if (argc == 4)
	{
	Dfile = argv[1];
	Lfile = argv[2];
	Hfile = argv[3];
	}
    else
	{
	(void) fprintf (stderr, BHASH_C_USAGE);
	return 1;
	}

    if (yyopen (Lfile))			/* Open the language file */
	return 1;
    yyinit ();				/* Set up for the parse */
    if (yyparse ())			/* Parse the language tables */
	exit (1);

    (void) sprintf (Cfile, "%s%s", Dfile, COUNTSUFFIX);
    (void) sprintf (Sfile, "%s%s", Dfile, STATSUFFIX);

    if (stat (Dfile, &dstat) < 0)
	{
	(void) fprintf (stderr, BHASH_C_NO_DICT, Dfile);
	    exit (1);
	}
    if (stat (Cfile, &cstat) < 0 || dstat.st_mtime > cstat.st_mtime)
	newcount ();

    if ((countf = fopen (Cfile, "r")) == NULL)
	{
	(void) fprintf (stderr, BHASH_C_NO_COUNT);
	exit (1);
	}
    hashsize = 0;
    if (fscanf (countf, "%d", &hashsize) != 1  ||  fclose (countf) == EOF)
	{
	(void) fprintf (stderr, BHASH_C_BAD_COUNT);
	exit (1);
	}
    if (hashsize == 0)
	{
	(void) fprintf (stderr, BHASH_C_ZERO_COUNT);
	exit (1);
	}
    readdict ();

    if ((statf = fopen (Sfile, "w")) == NULL)
	{
	(void) fprintf (stderr, CANT_CREATE, Sfile);
	exit (1);
	}

    for (i = 0; i < NSTAT; i++)
	stats[i] = 0;
    for (i = 0; i < hashsize; i++)
	{
	struct dent *   dp;

	dp = &hashtbl[i];
	if ((dp->flagfield & USED) != 0)
	    {
	    for (j = 0;  dp != NULL;  j++, dp = dp->next)
		{
		if (j >= NSTAT)
		    j = NSTAT - 1;
		stats[j]++;
		}
	    }
	}
    for (i = 0, j = 0, avg = 0;  i < NSTAT;  i++)
	{
	j += stats[i];
	avg += stats[i] * (i + 1);
	if (j == 0)
	    (void) fprintf (statf, "%d:\t%d\t0\t0.0\n", i + 1, stats[i]);
	else
	    (void) fprintf (statf, "%d:\t%d\t%d\t%f\n", i + 1, stats[i], j,
	      (double) avg / j);
	}
    (void) fclose (statf);

    filltable ();
    output ();
    return 0;
    }

static void output ()
    {
    register FILE *		houtfile;
    register struct dent *	dp;
    int				strptr;
    int				n;
    int				i;
    int				maxplen;
    int				maxslen;
    struct flagent *		fentry;

    if ((houtfile = fopen (Hfile, "wb")) == NULL)
	{
	(void) fprintf (stderr, CANT_CREATE, Hfile);
	return;
	}
    hashheader.stringsize = 0;
    hashheader.lstringsize = 0;
    hashheader.tblsize = hashsize;
    (void) fwrite ((char *) &hashheader, sizeof hashheader, 1, houtfile);
    strptr = 0;
    /*
    ** Put out the strings from the flags table.  This code assumes that
    ** the size of the hash header is a multiple of the size of ichar_t,
    ** and that any integer can be converted to an (ichar_t *) and back
    ** without damage.
    */
    maxslen = 0;
    for (i = numsflags, fentry = sflaglist;  --i >= 0;  fentry++)
	{
	if (fentry->stripl)
	    {
	    (void) fwrite ((char *) fentry->strip, fentry->stripl + 1,
	      sizeof (ichar_t), houtfile);
	    fentry->strip = (ichar_t *) strptr;
	    strptr += (fentry->stripl + 1) * sizeof (ichar_t);
	    }
	if (fentry->affl)
	    {
	    (void) fwrite ((char *) fentry->affix, fentry->affl + 1,
	      sizeof (ichar_t), houtfile);
	    fentry->affix = (ichar_t *) strptr;
	    strptr += (fentry->affl + 1) * sizeof (ichar_t);
	    }
	n = fentry->affl - fentry->stripl;
	if (n < 0)
	    n = -n;
	if (n > maxslen)
	    maxslen = n;
	}
    maxplen = 0;
    for (i = numpflags, fentry = pflaglist;  --i >= 0;  fentry++)
	{
	if (fentry->stripl)
	    {
	    (void) fwrite ((char *) fentry->strip, fentry->stripl + 1,
	      sizeof (ichar_t), houtfile);
	    fentry->strip = (ichar_t *) strptr;
	    strptr += (fentry->stripl + 1) * sizeof (ichar_t);
	    }
	if (fentry->affl)
	    {
	    (void) fwrite ((char *) fentry->affix, fentry->affl + 1,
	      sizeof (ichar_t), houtfile);
	    fentry->affix = (ichar_t *) strptr;
	    strptr += (fentry->affl + 1) * sizeof (ichar_t);
	    }
	n = fentry->affl - fentry->stripl;
	if (n < 0)
	    n = -n;
	if (n > maxplen)
	    maxplen = n;
	}
    /*
    ** Write out the string character type tables.
    */
    hashheader.strtypestart = strptr;
    for (i = 0;  i < hashheader.nstrchartype;  i++)
	{
	n = strlen (chartypes[i].name) + 1;
	(void) fwrite (chartypes[i].name, n, 1, houtfile);
	strptr += n;
	n = strlen (chartypes[i].deformatter) + 1;
	(void) fwrite (chartypes[i].deformatter, n, 1, houtfile);
	strptr += n;
	for (n = 0;
	  chartypes[i].suffixes[n] != '\0';
	  n += strlen (&chartypes[i].suffixes[n]) + 1)
	    ;
	n++;
	(void) fwrite (chartypes[i].suffixes, n, 1, houtfile);
	strptr += n;
	}
    hashheader.lstringsize = strptr;
    /* We allow one extra byte because missingletter() may add one byte */
    maxslen += maxplen + 1;
    if (maxslen > MAXAFFIXLEN)
	{
	(void) fprintf (stderr,
	  BHASH_C_BAFF_1 (MAXAFFIXLEN, maxslen - MAXAFFIXLEN));
	(void) fprintf (stderr, BHASH_C_BAFF_2);
	}
    /* Put out the dictionary strings */
    for (i = 0, dp = hashtbl;  i < hashsize;  i++, dp++)
	{
	if (dp->word == NULL)
	    dp->word = (char *) -1;
	else
	    {
	    n = strlen (dp->word) + 1;
	    (void) fwrite (dp->word, n, 1, houtfile);
	    dp->word = (char *) strptr;
	    strptr += n;
	    }
	}
    /* Pad file to a struct dent boundary for efficiency. */
    n = (strptr + sizeof hashheader) % sizeof (struct dent);
    if (n != 0)
	{
	n = sizeof (struct dent) - n;
	strptr += n;
	while (--n >= 0)
	    (void) putc ('\0', houtfile);
	}
    /* Put out the hash table itself */
    for (i = 0, dp = hashtbl;  i < hashsize;  i++, dp++)
	{
	if (dp->next != 0)
	    {
	    int		x;
	    x = dp->next - hashtbl;
	    dp->next = (struct dent *)x;
	    }
	else
	    {
	    dp->next = (struct dent *)-1;
	    }
#ifdef PIECEMEAL_HASH_WRITES
	(void) fwrite ((char *) dp, sizeof (struct dent), 1, houtfile);
#endif /* PIECEMEAL_HASH_WRITES */
	}
#ifndef PIECEMEAL_HASH_WRITES
    (void) fwrite ((char *) hashtbl, sizeof (struct dent), hashsize, houtfile);
#endif /* PIECEMEAL_HASH_WRITES */
    /* Put out the language tables */
    (void) fwrite ((char *) sflaglist,
      sizeof (struct flagent), numsflags, houtfile);
    hashheader.stblsize = numsflags;
    (void) fwrite ((char *) pflaglist,
      sizeof (struct flagent), numpflags, houtfile);
    hashheader.ptblsize = numpflags;
    /* Finish filling in the hash header. */
    hashheader.stringsize = strptr;
    rewind (houtfile);
    (void) fwrite ((char *) &hashheader, sizeof hashheader, 1, houtfile);
    (void) fclose (houtfile);
    }

static void filltable ()
    {
    struct dent *freepointer, *nextword, *dp;
    struct dent *hashend;
    int i;
    int overflows;
    
    hashend = hashtbl + hashsize;
    for (freepointer = hashtbl;
      (freepointer->flagfield & USED)  &&  freepointer < hashend;
      freepointer++)
	;
    overflows = 0;
    for (nextword = hashtbl, i = hashsize; i != 0; nextword++, i--)
	{
	if ((nextword->flagfield & USED) == 0)
	    continue;
	if (nextword->next >= hashtbl  &&  nextword->next < hashend)
	    continue;
	dp = nextword;
	while (dp->next)
	    {
	    if (freepointer >= hashend)
		{
		overflows++;
		break;
		}
	    else
		{
		*freepointer = *(dp->next);
		dp->next = freepointer;
		dp = freepointer;

		while ((freepointer->flagfield & USED)
		  &&  freepointer < hashend)
		    freepointer++;
		}
	    }
	}
    if (overflows)
	(void) fprintf (stderr, BHASH_C_OVERFLOW, overflows);
    }

#if MALLOC_INCREMENT == 0
VOID * mymalloc (size)
    unsigned int	size;
    {

    return malloc (size);
    }

/* ARGSUSED */
VOID * myrealloc (ptr, size, oldsize)
    VOID *		ptr;
    unsigned int	size;
    unsigned int	oldsize;
    {

    return realloc (ptr, size);
    }

void myfree (ptr)
    VOID *	ptr;
    {

    free (ptr);
    }

#else

VOID * mymalloc (size)		/* Fast, unfree-able variant of malloc */
    unsigned int	size;
    {
    VOID *		retval;
    static int		bytesleft = 0;
    static VOID *	nextspace;

    if (size < 4)
	size = 4;
    size = (size + 7) & ~7;	/* Assume doubleword boundaries are enough */
    if (bytesleft < size)
	{
	bytesleft = (size < MALLOC_INCREMENT) ? MALLOC_INCREMENT : size;
	nextspace = malloc ((unsigned) bytesleft);
	if (nextspace == NULL)
	    {
	    bytesleft = 0;
	    return NULL;
	    }
	}
    retval = nextspace;
    nextspace = (VOID *) ((char *) nextspace + size);
    bytesleft -= size;
    return retval;
    }

VOID * myrealloc (ptr, size, oldsize)
    VOID *		ptr;
    unsigned int	size;
    unsigned int	oldsize;
    {
    VOID *nptr;

    nptr = mymalloc (size);
    if (nptr == NULL)
	return NULL;
    (void) bcopy (ptr, nptr, oldsize);
    return nptr;
    }

/* ARGSUSED */
void myfree (ptr)
    VOID *		ptr;
    {
    }
#endif

static void readdict ()
    {
    struct dent		d;
    register struct dent * dp;
    struct dent *	lastdp;
    char		lbuf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];
    char		ucbuf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];
    FILE *		dictf;
    int			i;
    int			h;

    if ((dictf = fopen (Dfile, "r")) == NULL)
	{
	(void) fprintf (stderr, BHASH_C_CANT_OPEN_DICT);
	exit (1);
	}

    hashtbl =
      (struct dent *) calloc ((unsigned) hashsize, sizeof (struct dent));
    if (hashtbl == NULL)
	{
	(void) fprintf (stderr, BHASH_C_NO_SPACE);
	exit (1);
	}

    i = 0;
    while (fgets (lbuf, sizeof lbuf, dictf) != NULL)
	{
	if (!silent  &&  (i % 1000) == 0)
	    {
	    (void) fprintf (stderr, "%d ", i);
	    (void) fflush (stdout);
	    }
	i++;

	if (makedent (lbuf, sizeof lbuf, &d) < 0)
	    continue;

	h = hash (strtosichar (d.word, 1), hashsize);

	dp = &hashtbl[h];
	if ((dp->flagfield & USED) == 0)
	    {
	    *dp = d;
#ifndef NO_CAPITALIZATION_SUPPORT
	    /*
	    ** If it's a followcase word, we need to make this a
	    ** special dummy entry, and add a second with the
	    ** correct capitalization.
	    */
	    if (captype (d.flagfield) == FOLLOWCASE)
		{
		if (addvheader (dp))
		  exit (1);
		}
#endif
	    }
	else
	    {

	    /*
	    ** Collision.  Skip to the end of the collision
	    ** chain, or to a pre-existing entry for this
	    ** word.  Note that d.word always exists at
	    ** this point.
	    */
	    (void) strcpy (ucbuf, d.word);
	    chupcase (ucbuf);
	    while (dp != NULL)
		{
		if (strcmp (dp->word, ucbuf) == 0)
		    break;
#ifndef NO_CAPITALIZATION_SUPPORT
		while (dp->flagfield & MOREVARIANTS)
		    dp = dp->next;
#endif /* NO_CAPITALIZATION_SUPPORT */
		dp = dp->next;
		}
	    if (dp != NULL)
		{
		/*
		** A different capitalization is already in
		** the dictionary.  Combine capitalizations.
		*/
		if (combinecaps (dp, &d) < 0)
		    exit (1);
		}
	    else
		{
		/* Insert a new word into the dictionary */
		for (dp = &hashtbl[h];  dp->next != NULL;  )
		    dp = dp->next;
		lastdp = dp;
		dp = (struct dent *) mymalloc (sizeof (struct dent));
		if (dp == NULL)
		    {
		    (void) fprintf (stderr, BHASH_C_COLLISION_SPACE);
		    exit (1);
		    }
		*dp = d;
		lastdp->next = dp;
		dp->next = NULL;
#ifndef NO_CAPITALIZATION_SUPPORT
		/*
		** If it's a followcase word, we need to make this a
		** special dummy entry, and add a second with the
		** correct capitalization.
		*/
		if (captype (d.flagfield) == FOLLOWCASE)
		    {
		    if (addvheader (dp))
		      exit (1);
		    }
#endif
		}
	    }
	}
    if (!silent)
	(void) fprintf (stderr, "\n");
    (void) fclose (dictf);
    }

static void newcount ()
    {
    char		buf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];
#ifndef NO_CAPITALIZATION_SUPPORT
    ichar_t		ibuf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];
#endif
    register FILE *	d;
    register int	i;
#ifndef NO_CAPITALIZATION_SUPPORT
    ichar_t		lastibuf[sizeof ibuf / sizeof (ichar_t)];
    int			headercounted;
    int			followcase;
    register char *	cp;
#endif

    if (!silent)
	(void) fprintf (stderr, BHASH_C_COUNTING);

    if ((d = fopen (Dfile, "r")) == NULL)
	{
	(void) fprintf (stderr, BHASH_C_CANT_OPEN_DICT);
	exit (1);
	}

#ifndef NO_CAPITALIZATION_SUPPORT
    headercounted = 0;
    lastibuf[0] = 0;
#endif
    for (i = 0;  fgets (buf, sizeof buf, d);  )
	{
	if ((++i % 1000) == 0  &&  !silent)
	    {
	    (void) fprintf (stderr, "%d ", i);
	    (void) fflush (stdout);
	    }
#ifndef NO_CAPITALIZATION_SUPPORT
	cp = index (buf, hashheader.flagmarker);
	if (cp != NULL)
	    *cp = '\0';
	if (strtoichar (ibuf, buf, INPUTWORDLEN * sizeof (ichar_t), 1))
	    (void) fprintf (stderr, WORD_TOO_LONG (buf));
	followcase = (whatcap (ibuf) == FOLLOWCASE);
	upcase (ibuf);
	if (icharcmp (ibuf, lastibuf) != 0)
	    headercounted = 0;
	else if (!headercounted)
	    {
	    /* First duplicate will take two entries */
	    if ((++i % 1000) == 0  &&  !silent)
		{
		(void) fprintf (stderr, "%d ", i);
		(void) fflush (stdout);
		}
	    headercounted = 1;
	    }
	if (!headercounted  &&  followcase)
	    {
	    /* It's followcase and the first entry -- count again */
	    if ((++i % 1000) == 0  &&  !silent)
		{
		(void) fprintf (stderr, "%d ", i);
		(void) fflush (stdout);
		}
	    headercounted = 1;
	    }
	(void) icharcpy (lastibuf, ibuf);
#endif
	}
    (void) fclose (d);
    if (!silent)
	(void) fprintf (stderr, BHASH_C_WORD_COUNT, i);
    if ((d = fopen (Cfile, "w")) == NULL)
	{
	(void) fprintf (stderr, CANT_CREATE, Cfile);
	exit (1);
	}
    (void) fprintf (d, "%d\n", i);
    (void) fclose (d);
    }

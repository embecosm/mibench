#ifndef lint
static char Rcs_Id[] =
    "$Id: icombine.c,v 2.24 1994/01/25 07:11:35 geoff Exp $";
#endif

#define MAIN

/*
 * icombine:  combine multiple ispell dictionary entries into a single 
 *            entry with the options of all entries
 *
 * The original version of this program was written by Gary Puckering at
 * Cognos, Inc.  The current version is a complete replacement, created by
 * reducing Pace Willisson's buildhash program.  By using routines common
 * to buildhash and ispell, we can be sure that the rules for combining
 * capitalizations are compatible.
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
 * $Log: icombine.c,v $
 * Revision 2.24  1994/01/25  07:11:35  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <ctype.h>
#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "msgs.h"

char *		Lfile;			/* Language-description file */

int		main P ((int argc, char * argv[]));
static void	usage P ((void));
VOID *		mymalloc P ((unsigned int size));
VOID *		myrealloc P ((VOID * ptr, unsigned int size,
		  unsigned int oldsize));
void		myfree P ((VOID * ptr));
static void	combinedict P ((void));
static void	combineout P ((void));

int main (argc, argv)
    int		argc;
    char *	argv[];
    {
    char *	argp;
    char *	preftype = NULL;

    while (argc > 1  &&  argv[1][0] == '-')
	{
	argc--;
	argv++;
	switch (argv[0][1])
	    {
	    case 'T':
		argp = (*argv)+2;
		if (*argp == '\0')
		    {
		    argv++; argc--;
		    if (argc == 0)
			usage ();
		    argp = *argv;
		    }
		preftype = argp;
		break;
	    default:
		usage ();
		break;
	    }
	}

    if (argc > 1)			/* Figure out what language to use */
	Lfile = argv[1];
    else
	Lfile = DEFLANG;

    if (yyopen (Lfile))			/* Open the language file */
      return 1;
    yyinit ();				/* Set up for the parse */
    if (yyparse ())			/* Parse the language tables */
	exit (1);

    if (preftype != NULL)
	{
	defdupchar = findfiletype (preftype, 1, (int *) NULL);
	if (defdupchar < 0
	  &&  strcmp (preftype, "tex") != 0
	  &&  strcmp (preftype, "nroff") != 0)
	    {
	    (void) fprintf (stderr, ICOMBINE_C_BAD_TYPE, preftype);
	    exit (1);
	    }
	}
    if (defdupchar < 0)
	defdupchar = 0;

    combinedict ();			/* Combine words */

    return 0;
    }

static void usage ()
    {

    (void) fprintf (stderr, ICOMBINE_C_USAGE);
    exit (1);
    }

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

static void combinedict ()
    {
    struct dent		d;
    register struct dent * dp;
    char		lbuf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];
    ichar_t		ucbuf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];
    ichar_t		lastbuf[INPUTWORDLEN + MAXAFFIXLEN + 2 * MASKBITS];

    lastbuf[0] = '\0';
    hashtbl = (struct dent *) mymalloc (sizeof (struct dent));
    hashtbl->flagfield = 0;
    hashtbl->word = 0;
    while (fgets (lbuf, sizeof lbuf, stdin) != NULL)
	{
	if (ichartostr (lbuf, strtosichar (lbuf, 0), sizeof lbuf, 1))
	    (void) fprintf (stderr, WORD_TOO_LONG (lbuf));
	if (makedent (ichartosstr (strtosichar (lbuf, 0), 1),
	    ICHARTOSSTR_SIZE, &d)
	  < 0)
	    continue;

	if (strtoichar (ucbuf, d.word, sizeof ucbuf, 1))
	    (void) fprintf (stderr, WORD_TOO_LONG (lbuf));
	upcase (ucbuf);
	if (icharcmp (ucbuf, lastbuf) != 0)
	    {
	    /*
	    ** We have a new word.  Put the old one out.
	    */
	    combineout ();
	    (void) icharcpy (lastbuf, ucbuf);
	    }

	dp = hashtbl;
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
	    ** A different capitalization is already in
	    ** the dictionary.  Combine capitalizations.
	    */
	    if (combinecaps (dp, &d) < 0)
	      exit (1);
	    }
	}
    combineout ();
    }

static void combineout ()
    {
    register struct dent *	ndp;
    register struct dent *	tdp;

    /*
    ** Put out the dictionary entry on stdout in text format,
    ** freeing it as we go.
    **/
    if (hashtbl->flagfield & USED)
	{
	for (tdp = hashtbl;  tdp != NULL;  tdp = ndp)
	    {
	    toutent (stdout, tdp, 0);
	    myfree (tdp->word);
	    ndp = tdp->next;
#ifndef NO_CAPITALIZATION_SUPPORT
	    while (tdp->flagfield & MOREVARIANTS)
		{
		if (tdp != hashtbl)
		    myfree ((char *) tdp);
		tdp = ndp;
		if (tdp->word)
		    myfree (tdp->word);
		ndp = tdp->next;
		}
#endif
	    if (tdp != hashtbl)
		myfree ((char *) tdp);
	    }
	}
    hashtbl->flagfield = 0;
    hashtbl->word = NULL;
    }

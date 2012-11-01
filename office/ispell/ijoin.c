#ifndef lint
static char Rcs_Id[] =
    "$Id: ijoin.c,v 1.6 1994/10/18 04:03:21 geoff Exp $";
#endif

/*
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
 * "Join" command for ispell.
 *
 * This command is a complete reimplementation of the UNIX "join"
 * command, except that fields cannot be separated by a newline, it
 * can handle lines of unlimited length, and the preceding sort can
 * treat characters as either signed or unsigned.
 *
 * Usage:
 *
 *	ijoin [options] file1 file2
 *
 * See the UNIX "join" manual page for option descriptions.  Only
 * nonstandard options are described here.
 *
 * Either file1 or file2 may be "-", in which case the standard input
 * is used for that file.
 *
 * Normally, ijoin uses "strcmp" to compare fields.  This is the
 * correct thing to do on most systems if you are using the
 * system-provided "sort" command to sort the input files before
 * feeding them to ijoin.  In some cases, however, the sort command
 * you use will disagree with strcmp about how to handle characters
 * with the high bit set.  If this is the case, you can specify the
 * "-s" (signed comparisons) or "-u" (unsigned comparisons) option to
 * force ijoin to conform to the method used by the sort program.
 * This is only necessary if one of the input files contains 8-bit
 * characters in the field that is being joined on.
 *
 * On some older machines with non-ANSI compilers, the "-s" option
 * will be ineffective because characters default to unsigned.
 * However, this option should be unnecessary on those machines, so no
 * harm will be done.
 */

/*
 * $Log: ijoin.c,v $
 * Revision 1.6  1994/10/18  04:03:21  geoff
 * Fix a couple of bugs, one where the last field on a line could be
 * output incorrectly, and one where fields from the wrong file could be
 * output.
 *
 * Revision 1.5  1994/01/25  07:11:36  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <stdio.h>
#include "config.h"
#include "ispell.h"
#include "proto.h"
#include "fields.h"

#ifdef __STDC__
#define SIGNED	signed
#else /* __STDC */
#define SIGNED
#endif /* __STDC */

int		main P ((int argc, char * argv[])); /* Join files */
static void	usage P ((void));	/* Issue a usage message */
static void	dojoin P ((void));	/* Actually perform the join */
static void	full_output P ((field_t * line1, field_t * line2));
					/* Output everything from both lines */
static void	selected_output P ((field_t * line1, field_t * line2));
					/* Output selected fields */
static int	strscmp P ((SIGNED char * a, SIGNED char * b));
					/* Signed version of strcmp */
static int	strucmp P ((unsigned char * a, unsigned char * b));
					/* Unsigned version of strcmp */

typedef struct
    {
    int		file;			/* Number of file to output from */
    int		field;			/* Number of field to output */
    }
		outlist_t;		/* Output description list */

static int		(*compare) () = strcmp; /* Comparison function */
static char *		emptyfield = ""; /* Use this to replace empty fields */
static FILE *		file1;		/* First file to join */
static FILE *		file2;		/* Second file to join */
static int		join1field = 0;	/* Field to join file 1 on */
static int		join2field = 0;	/* Field to join file 2 on */
static int		maxf[2] = {0, 0}; /* Max field to parse in each file */
static outlist_t *	outlist = NULL;	/* List of fields to write */
static int		outlistsize;	/* Number of elements in outlist */
static int		runs = FLD_RUNS; /* Set if runs of tabchar same as 1 */
static char *		tabchar = " \t"; /* Field separator character(s) */
static int		unpairable1 = 0; /* NZ if -a1 */
static int		unpairable2 = 0; /* NZ if -a2 */

int main (argc, argv)			/* Join files */
    int			argc;		/* Argument count */
    char *		argv[];		/* Argument vector */
    {
    
    while (argc > 3  &&  argv[1][0] == '-')
	{
	argc--;
	argv++;
	switch (argv[0][1])
	    {
	    case 'a':			/* produce output for unpairables */
		if (argv[0][2] == '1')
		    unpairable1 = 1;
		else if (argv[0][2] == '2')
		    unpairable2 = 1;
		else if (argv[0][2] == '\0')
		    unpairable1 = unpairable2 = 1;
		else
		    usage ();
		break;
	    case 'e':			/* Replace empty fields with this */
		argc--;
		argv++;
		emptyfield = *argv;
		break;
	    case 'j':			/* Specify field to join on */
		if (argv[0][2] == '1')
		    join1field = atoi (argv[1]) - 1;
		else if (argv[0][2] == '2')
		    join2field = atoi (argv[1]) - 1;
		else if (argv[0][2] == '\0')
		    join1field = join2field = atoi (argv[1]) - 1;
		else
		    usage ();
		argc--;
		argv++;
		break;
	    case 'o':			/* Specify output list */
		/*
		 * We will assume that all remaining switch arguments
		 * are used to describe the output list.  This will
		 * occasionally result in malloc'ing a few too many
		 * elements, but no real harm will be done.
		 */
		outlist =
		  (outlist_t *) malloc ((argc - 3) * sizeof (outlist_t));
		if (outlist == NULL)
		    {
		    (void) fprintf (stderr, "ijoin:  out of memory!\n");
		    return 1;
		    }
		for (outlistsize = 0, argc--, argv++;
		  argc > 2  &&  (argv[0][0] == '1'  ||  argv[0][0] == '2')
		    &&  argv[0][1] == '.';
		  argc--, argv++, outlistsize++)
		    {
		    outlist[outlistsize].file = argv[0][0] - '0';
		    outlist[outlistsize].field = atoi (&argv[0][2]) - 1;
		    if (maxf[outlist[outlistsize].file - 1]
		      <= outlist[outlistsize].field)
			maxf[outlist[outlistsize].file - 1] =
			  outlist[outlistsize].field + 1;
		    }
		argc++;			/* Un-do arg that stopped us */
		argv--;
		break;
	    case 't':
		tabchar = &argv[0][2];
		runs &= ~FLD_RUNS;
		break;
	    case 's':
		compare = strscmp;
		break;
	    case 'u':
		compare = strucmp;
		break;
	    default:
		usage ();
		break;
	    }
	}
    if (argc != 3)
	usage ();
    if (strcmp (argv[1], "-") == 0)
	file1 = stdin;
    else
	{
	file1 = fopen (argv[1], "r");
	if (file1 == NULL)
	    perror (argv[1]);
	}
    file2 = fopen (argv[2], "r");
    if (file2 == NULL)
	perror (argv[2]);
    if (file1 == NULL  ||  file2 == NULL)
	return 1;
    dojoin ();
    return 0;
    }

static void usage ()			/* Issue a usage message */
    {

    (void) fprintf (stderr,
      "Usage:  ijoin [-an] [-e s] [-jn m] [-o n.m ...] [-tc] file1 file2\n");
    exit (1);
    }

static void dojoin ()			/* Actually perform the join */
    {
    int			comparison;	/* Result of comparing the lines */
    long		file2pos;	/* Position file 2 started at */
    register field_t *	line1;		/* Line from file 1 */
    register field_t *	line2;		/* Line from file 2 */
    int			pairable;	/* NZ if lines can be paired */
    int			skip2;		/* No. of "unpairable" 2's to skip */

    runs |= FLD_NOSHRINK;		/* Don't do excessive reallocations */
    field_line_inc = BUFSIZ;		/* Allocate line bfr in huge chunks */
    line1 = fieldread (file1, tabchar, runs, maxf[0]);
    file2pos = ftell (file2);
    skip2 = 0;
    if (file2pos == -1)
	{
	(void) fprintf (stderr, "ijoin:  Can't seek file ");
	perror ("2");
	exit (1);
	}
    line2 = fieldread (file2, tabchar, runs, maxf[1]);
    while (line1 != NULL  ||  line2 != NULL)
	{
	/*
	 * Do a little work to reduce the number of calls to realloc, at
	 * the expense of slightly-increased memory usage.
	 */
	if (line1 != NULL  &&  line1->nfields >= field_field_inc)
	    field_field_inc = line1->nfields + 1;
	if (line2 != NULL  &&  line2->nfields >= field_field_inc)
	    field_field_inc = line2->nfields + 1;
	/*
	 * Determine if the lines can be paired.
	 */
	pairable = 1;
	if (line1 == NULL)
	    {
	    pairable = 0;
	    comparison = 1;		/* This causes file 2 to advance */
	    }
	else if (join1field >= line1->nfields)
	    {
	    pairable = 0;
	    comparison = -1;		/* This causes file 1 to advance */
	    }
	if (line2 == NULL)
	    {
	    pairable = 0;
	    comparison = -1;		/* This causes file 1 to advance */
	    }
	else if (join2field >= line2->nfields)
	    {
	    pairable = 0;
	    comparison = 1;		/* This causes file 2 to advance */
	    }
	if (pairable)
	    {
	    comparison = (*compare) (line1->fields[join1field],
	      line2->fields[join2field]);
	    pairable = (comparison == 0);
	    }
	if (pairable)
	    {
	    /*
	     * The two lines can be paired.  Produce output.
	     */
	    if (outlist == NULL)
		full_output (line1, line2);
	    else
		selected_output (line1, line2);
	    }
	/*
	 * Advance through the files
	 */
	if (comparison < 0)
	    {
	    if (unpairable1)
		{
		if (outlist == NULL)
		    (void) fieldwrite (stdout, line1, tabchar[0]);
		else
		    selected_output (line1, (field_t *) NULL);
		}
	    fieldfree (line1);
	    line1 = fieldread (file1, tabchar, runs, maxf[0]);
	    }
	else if (comparison > 0)
	    {
	    if (skip2 > 0)
		skip2--;
	    else if (unpairable2)
		{
		if (outlist == NULL)
		    (void) fieldwrite (stdout, line2, tabchar[0]);
		else
		    selected_output ((field_t *) NULL, line2);
		}
	    fieldfree (line2);
	    file2pos = ftell (file2);
	    line2 = fieldread (file2, tabchar, runs, maxf[1]);
	    }
	else
	    {
	    /*
	     * Here's the tricky part.  We have to advance file 2
	     * until comparisons fail, and then back it up and advance
	     * file 1.
	     */
	    skip2++;
	    fieldfree (line2);
	    line2 = fieldread (file2, tabchar, runs, maxf[1]);
	    if (line2 == NULL
	      ||  join2field >= line2->nfields
	      ||  (*compare) (line1->fields[join1field],
		  line2->fields[join2field])
		!= 0)
		{
		(void) fseek (file2, file2pos, 0);
		fieldfree (line2);
		line2 = fieldread (file2, tabchar, runs, maxf[1]);
		fieldfree (line1);
		line1 = fieldread (file1, tabchar, runs, maxf[0]);
		if (line1 != NULL  &&  line2 != NULL
		  &&  join1field < line1->nfields
		  &&  join2field < line2->nfields
		  &&  (*compare) (line1->fields[join1field],
			line2->fields[join2field])
		    == 0)
		    skip2 = 0;
		}
	    }
	}
    }

static void full_output (line1, line2)	/* Output everything from both lines */
    register field_t *	line1;		/* Line from file 1 */
    register field_t *	line2;		/* Line from file 2 */
    {
    register int	fieldno;	/* Number of field being handled */

    (void) fputs (line1->fields[join1field], stdout);
    for (fieldno = 0;  fieldno < line1->nfields;  fieldno++)
	{
	if (fieldno == join1field)
	    continue;
	(void) putchar (tabchar[0]);
	if (line1->fields[fieldno][0] == '\0')
	    (void) fputs (emptyfield, stdout);
	else
	    (void) fputs (line1->fields[fieldno], stdout);
	}
    for (fieldno = 0;  fieldno < line2->nfields;  fieldno++)
	{
	if (fieldno == join2field)
	    continue;
	(void) putchar (tabchar[0]);
	if (line2->fields[fieldno][0] == '\0')
	    (void) fputs (emptyfield, stdout);
	else
	    (void) fputs (line2->fields[fieldno], stdout);
	}
    (void) putchar ('\n');
    }

static void	selected_output (line1, line2) /* Output selected fields */
    field_t *		line1;		/* Line from file 1 */
    field_t *		line2;		/* Line from file 2 */
    {
    register field_t *	cline;		/* Current line being handled */
    register int	listno;		/* Number of output list entry */
    
    for (listno = 0;  listno < outlistsize;  listno++)
	{
	if (listno != 0)
	    (void) putchar (tabchar[0]);
	if (outlist[listno].file == 1)
	    cline = line1;
	else
	    cline = line2;
	if (cline == NULL
	  ||  outlist[listno].field >= cline->nfields
	  ||  cline->fields[outlist[listno].field][0] == '\0')
	    (void) fputs (emptyfield, stdout);
	else
	    (void) fputs (cline->fields[outlist[listno].field], stdout);
	}
    (void) putchar ('\n');
    }

static int strscmp (a, b)		/* Compare signed strings */
    register SIGNED char * a;		/* First string to compare */
    register SIGNED char * b;		/* Second string to compare */
    {

    while (*a != '\0')
	{
	if (*a++ != *b++)
	    return *--a - *--b;
	}
    return *a - *b;
    }

static int strucmp (a, b)		/* Compare unsigned strings */
    register unsigned char * a;		/* First string to compare */
    register unsigned char * b;		/* Second string to compare */
    {

    while (*a != '\0')
	{
	if (*a++ != *b++)
	    return *--a - *--b;
	}
    return *a - *b;
    }

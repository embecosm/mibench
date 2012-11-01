#ifndef lint
static char Rcs_Id[] =
    "$Id: fix8bit.c,v 1.3 1994/01/25 07:12:26 geoff Exp $";
#endif

/*
 * Copyright 1993, Geoff Kuenning, Granada Hills, CA
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
 * This is a stupid little program that can be used to convert 8-bit
 * characters to and from backslashed escape sequences.  It is usually
 * more efficient to do this to affix files than to uuencode them for
 * transport.  Ispell will read affix files in either format, so it is
 * merely personal preference as to which form to use.
 *
 * Usage:
 *
 *	fix8bit {-7 | -8} < infile > outfile
 *
 * One of -7 and -8 must be specified.  If -7 is given, any character
 * sequence that is not standard printable ASCII will be converted
 * into a backslashed octal sequence.  If -8 is given, any backslashed
 * octal or hex sequence will be converted into the equivalent 8-bit
 * character.
 *
 * This program is not very smart.  In particular, it makes no attempt
 * to understand comments, quoted strings, or similar constructs in
 * which you might not want conversion to take place.  I suggest that
 * you "diff" the input against the output, and if you don't like the
 * result, correct it by hand.
 */

/*
 * $Log: fix8bit.c,v $
 * Revision 1.3  1994/01/25  07:12:26  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <stdio.h>

int		main ();	/* Convert to/from 8-bit sequences */
static void	usage ();	/* Issue a usage message */
static void	to7bit ();	/* Convert from 8-bit sequences */
static void	to8bit ();	/* Convert to 8-bit sequences */

extern void	exit ();	/* Terminate program */

int main (argc, argv)		/* Convert to/from 8-bit sequences */
    int			argc;	/* Argument count */
    char *		argv[];	/* Argument vector */
    {
    
    if (argc != 2)
	usage ();
    if (strcmp (argv[1], "-7") == 0)
	to7bit ();
    else if (strcmp (argv[1], "-8") == 0)
	to8bit ();
    else
	usage ();
    return 0;
    }

static void usage ()		/* Issue a usage message */
    {

    (void) fprintf (stderr, "Usage:  fix8bit {-7 | -8} < infile > outfile\n");
    exit (1);
    }

static void to7bit ()		/* Convert from 8-bit sequences */
    {
    int			ch;	/* Next character read from input */

    while ((ch = getchar ()) != EOF)
	{
	ch &= 0xFF;
	if (ch >= 0200)
	    (void) printf ("\\%3.3o", (unsigned) ch);
	else
	    (void) putchar (ch);
	}
    }

static void to8bit ()		/* Convert to 8-bit sequences */
    {
    int			backch;	/* Backslashed character being built */
    int			ch;	/* Next character read from input */

    while ((ch = getchar ()) != EOF)
	{
	ch &= 0xFF;
	if (ch != '\\')
	    (void) putchar (ch);
	else
	    {
	    /*
	     * Collect a backslashed character.
	     */
	    ch = getchar ();
	    switch (ch)
		{
		case EOF:
		    (void) putchar ('\\');
		    return;
		case 'x':
		case 'X':
		    backch = ch;
		    ch = getchar ();
		    if (ch >= '0'  &&  ch <= '9')
			backch = ch - '0';
		    else if (ch >= 'a'  &&  ch <= 'f')
			backch = ch - 'a' + 0xA;
		    else if (ch >= 'A'  &&  ch <= 'F')
			backch = ch - 'A' + 0xA;
		    else
			{
			(void) putchar ('\\');
			(void) putchar (backch);
			(void) putchar (ch);
			break;
			}
		    ch = getchar ();
		    if (ch >= '0'  &&  ch <= '9')
			backch = (backch << 4) | (ch - '0');
		    else if (ch >= 'a'  &&  ch <= 'f')
			backch = (ch << 4) - 'a' + 0xA;
		    else if (ch >= 'A'  &&  ch <= 'F')
			backch = (ch << 4) - 'A' + 0xA;
		    else
			{
			(void) putchar (backch);
			(void) putchar (ch);
			break;
			}
		    (void) putchar (backch);
		    break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		    backch = ch - '0';
		    ch = getchar ();
		    if (ch >= '0'  &&  ch <= '7')
			backch = (backch << 3) | (ch - '0');
		    else
			{
			(void) putchar (backch);
			(void) putchar (ch);
			break;
			}
		    ch = getchar ();
		    if (ch >= '0'  &&  ch <= '7')
			backch = (backch << 3) | (ch - '0');
		    else
			{
			(void) putchar (backch);
			(void) putchar (ch);
			break;
			}
		    (void) putchar (backch);
		    break;
		    break;
		default:
		    (void) putchar ('\\');
		    (void) putchar (ch);
		    break;
		}
	    }
	}
    }

#ifndef lint
static char Rcs_Id[] =
    "$Id: sq.c,v 1.12 1994/01/25 07:12:09 geoff Exp $";
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
 * $Log: sq.c,v $
 * Revision 1.12  1994/01/25  07:12:09  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

#include <stdio.h>

#ifdef __STDC__
#define P(x)	x
#else /* __STDC__ */
#define P(x)	()
#endif /* __STDC__ */

int		main P ((int argc, char * argv[]));
static void	trunc P ((char * word, char * prev));

/*
 * The following table encodes prefix sizes as a single character.  A
 * matching table will be found in unsq.c.
 */
static char size_encodings[] =
    {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',	/* 00-09 */
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',	/* 10-19 */
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',	/* 20-29 */
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',	/* 30-39 */
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',	/* 40-49 */
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',	/* 50-59 */
    'y', 'z'						/* 60-61 */
    };

#define MAX_PREFIX	(sizeof (size_encodings) - 1)

int main (argc, argv)
    int		argc;
    char *	argv[];
    {
    char	word[257];
    static char	prev[257] = "";

    while (gets (word) != NULL)
	trunc (word, prev);
    return 0;
    }

static void trunc (word, prev) 
    char *		word;
    char *		prev;
    {
    register char *	wordp;
    register char *	prevp;
    register int	same_count;

    wordp = word;
    prevp = prev;
    for (same_count = 0;  *wordp == *prevp++;  ++wordp, ++same_count)
	;
    if (same_count>MAX_PREFIX)
	same_count = MAX_PREFIX;
    (void) putchar (size_encodings[same_count]);
    (void) puts (wordp);
    (void) strcpy (prev, word);
    }


/*
 *   killdups.c
 *	Remove duplicate quoted strings in a file
 *	After each outputted quoted string, adds the PLACEHOLDER
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if 0
#define	PLACEHOLDER	"No translation\n"
#else
#define	PLACEHOLDER	""
#endif

struct strbuf {
	char		*s;
	struct strbuf	*l, *r;
}		*top;


main ()
{
	char	*str, *getstr();

	while (str = getstr()) {
		if (!inlistadd (str)) {
			putstr (str);
		}
	}
}


char *
getstr ()
{
	static char	buf[4096];
	char		*bp;
	int		c;
	int		bslash;
	char		*sbuf;

	bp = buf;
	while ((c = getchar()) != EOF  &&  c != '"')
		;
	if (c == EOF)
		return NULL;
	bslash = 0;
	while ((c = getchar()) != EOF) {
		if (bslash) {
			*bp++ = c;
			bslash = 0;
		} else {
			if (c == '"')
				break;
			if (c == '\\')
				bslash = 1;
			*bp++ = c;
		}
		if (bp - buf >= sizeof(buf)) {
			fprintf (stderr, "String too long\n");
			exit (2);
		}
	}
	*bp++ = '\0';
	sbuf = malloc (bp - buf);
	if (!sbuf) {
		fprintf (stderr, "Out of memory\n");
		exit (1);
	}
	strcpy (sbuf, buf);
	return sbuf;
}

int
putstr (s)
char		*s;
{
	printf ("\"%s\"\n%s\n", s, PLACEHOLDER);
}

int
inlistadd (s)
char		*s;
{
	int		delta;
	struct strbuf	*list;

	if (top == NULL) {
		newentry (&top, s);
		return 0;
	}
	list = top;
	for ( ; ; ) {
		delta = strcmp (s, list->s);
		if (delta == 0)
			return 1;	/* Already in list */
		if (delta > 0) {
			if (list->r == NULL) {
				newentry (&list->r, s);
				return 0;
			} else
				list = list->r;
		} else {
			if (list->l == NULL) {
				newentry (&list->l, s);
				return 0;
			} else
				list = list->l;
		}
	}
}

int
newentry (alist, s)
struct strbuf	**alist;
char		*s;
{
	*alist = (struct strbuf *)malloc (sizeof(struct strbuf));
	if (*alist == NULL) {
		fprintf (stderr, "Out of memory\n");
		exit (3);
	}
	(*alist)->s = s;
	(*alist)->l = (*alist)->r = NULL;
}

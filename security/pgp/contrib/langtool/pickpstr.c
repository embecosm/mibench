/*
 *   pickpstr.c
 *	Read a set of files.
 *	Find all strings of the form PSTR("...").
 *	Output them, minus the enclosing PSTR( and ) to stdout.
 */

#include <stdio.h>
#define	STR	"LANG(\""

main (ac, av)
char	**av;
{
	int	copying, match, bslash;
	FILE	*f;
	int	c;

	if (ac < 2) {
		fprintf (stderr, "Usage: %s file [...]\n", av[0]);
		exit (1);
	}

	while (*++av) {
		if ((f = fopen (*av, "r")) == NULL) {
			fprintf (stderr, "Unable to open file %s, skipping\n");
			continue;
		}
		match = 0;
		copying = 0;
		while ((c = getc(f)) != EOF) {
			if (!copying) {
				if (c == STR[match]) {
					if (++match == strlen(STR)) {
						copying = 1;
						match = 0;
						bslash = 0;
						putchar (c);
					}
				} else
					match = 0;
			} else {
				if (!bslash) {
					if (c == '"') {
						putchar (c);
						putchar ('\n');
						putchar ('\n');
						copying = 0;
					} else if (c == '\\') {
						bslash = 1;
						putchar (c);
					} else
						putchar (c);
				} else {
					putchar (c);
					bslash = 0;
				}
			}
		}
		fclose (f);
	}
	exit (0);
}

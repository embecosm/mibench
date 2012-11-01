/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*$Header: /home/mguthaus/.cvsroot/mibench/telecomm/gsm/tst/cod2lin.c,v 1.1.1.1 2000/11/06 19:54:26 mguthaus Exp $*/

#include <stdio.h>
#include <assert.h>

#include	"gsm.h"
#include	"proto.h"

char  * pname;

int	debug      = 0;
int	verbosity  = 0;
int	fast       = 0;
int	error      = 0;

usage P0()
{
	fprintf(stderr, "Usage: %s [-vF] [files...]\n", pname);
	exit(1);
}

void process P2((f, filename), FILE * f, char * filename)
{
	gsm_frame	buf;
	gsm_signal	source[160];

	int		cc;
	gsm		r;

	(void)memset(source, 0x00, sizeof(source));

	if (!(r = gsm_create())) {
		perror("gsm_create");
		error = 1;
		return ;
	}
	gsm_option(r, GSM_OPT_VERBOSE, &verbosity);
	gsm_option(r, GSM_OPT_FAST,   &fast);
	for (;;) {
		cc = fread((char *)source, sizeof(*source), 76, f);
		if (cc == 0) {
			gsm_destroy(r);
			return;
		}
		if (cc != 76) {
			error = 1;
			fprintf(stderr,
				"%s: %s -- %d trailing bytes ignored\n",
				pname, filename, cc);
			gsm_destroy(r);
			return;
		}

		gsm_implode(r, source, buf);
		gsm_decode(r, buf, source);

		if (write(1, source, sizeof(source)) != sizeof(source)) {
			perror("write");
			error = 1;
			gsm_destroy(r);
			return;
		}
	}
}

main P2((ac, av), int ac, char ** av)
{
	int 		opt;
	extern char   * optarg;
	extern int	optind;

	FILE		* f;

	if (!(pname = av[0])) pname = "cod2out";

	while ((opt = getopt(ac, av, "vF")) != EOF) switch (opt) {
	case 'v': verbosity++; 	  break;
	case 'F': fast++;	  break;
	default:  usage();
	}

	ac -= optind;
	av += optind;

	if (!ac) process(stdin, "*stdin*");
	else for (; *av; av++) {
		if (!(f = fopen(*av, "r"))) perror(*av);
		else {
			process(f, *av);
			fclose(f);
		}
	}

	exit(error);
}

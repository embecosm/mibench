/*
 *	language.c - Foreign language translation for PGP
 *	Finds foreign language "subtitles" for English phrases 
 *	in external foriegn language text file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "usuals.h"
#ifndef LANGTOOL
#include "fileio.h"
#include "language.h"
#include "pgp.h"
#else
#define MAX_PATH	255
boolean verbose;
long fsize();
#endif

char langfile[80] = "language.txt";
#define LANG_INDEXFILE	"language.idx"

#define	STRBUFSIZE		2048

char language[16] = "en";	/* The language code, defaults to English */
static char	*strbuf;
static char	lang[16];	/* readstr sets this to the language id of the msg it last read */
static int	subtitles_available = 0;
static int line = 0;
static int errcount = 0;
/*	subtitles_available is used to determine if we know whether the special
	subtitles_file exists.  subtitles_available has the following values:
	0  = first time thru, we don't yet know if subtitles_file exists.
	1  = we have already determined that subtitles_file exists.
	-1 = we have already determined that subtitles_file does not exist.
*/

static void error(char *);

#define	NEWLINE		0
#define	COMMENT		1
#define	INSTRING	2
#define	ESCAPE		3
#define	IDENT		4
#define	DONE		5
#define	ERROR		6
#define	ERR1		7

/* Look for and return a quoted string from the file.
 * If nlabort is true, return failure if we find a blank line
 * before we find the opening quote.
 */
static char	*
readstr (FILE *f, char *buf, int nlabort)
{
	int	c, d;
	char *p = buf;
	int state = NEWLINE;
	int i = 0;
	
	while ((c = getc(f)) != EOF) {
		if (c == '\r')
			continue;
		/* line numbers are only incremented when creating index file */
		if (line && c == '\n')
			++line;
		switch (state) {
		  case NEWLINE:
			switch(c) {
			  case '#': state = COMMENT; break;
			  case '"': state = INSTRING; break;
			  case '\n':
				if (nlabort) {
					*buf = '\0';
					return(buf);
				}
			  default:
				if (i == 0 && isalnum(c)) {
					state = IDENT;
					lang[i++] = c;
					break;
				}
				if (!isspace(c)) {
					error("syntax error\n");
					state = ERROR;
				}
			}
			break;
		  case COMMENT:
			if (c == '\n')
				state = NEWLINE;
			break;
		  case INSTRING:
			switch(c) {
			  case '\\': state = ESCAPE; break;
			  case '"': state = DONE; break;
			  default: *p++ = c;
			}
			break;
		case ESCAPE:
			switch (c) {
			  case 'n':	*p++ = '\n';	break;
			  case 'r':	*p++ = '\r';	break;
			  case 't':	*p++ = '\t';	break;
			  case 'e':	*p++ = '\033';	break;
			  case 'a':	*p++ = '\007';	break;
			  case '#':
			  case '"':
			  case '\\':	*p++ = c; break;
			  case '\n':	break;
			  case '0':
			  case '1':
			  case '2':
			  case '3':
			  case '4':
			  case '5':
			  case '6':
			  case '7':
				/* ANSI C rules: up to 3 octal digits */
				d = c - '0';
				if ((c = getc(f)) >= '0' && c <= '7') {
					d = (d<<3) + (c-'0');
					if ((c = getc(f)) >= '0' && c <= '7')
						d = (d<<3) + (c-'0');
					else
						ungetc(c, f);
				} else {
					ungetc(c, f);
				}
				*p++ = d;
				break;
			  default:
				error("illegal escape sequence: ");
				fprintf(stderr, "'\\%c'\n", c);
				break;
			}
			state = INSTRING;
			break;
		  case IDENT:		/* language identifier */
			if (c == ':') {
				state = NEWLINE;
				break;
			}
			if (c == '\n' && strncmp(lang, "No translation", 14) == 0)
			{
				i = 0;
				state = NEWLINE;
				break;
			}
			lang[i++] = c;
			if (i == 15 || !isalnum(c) && !isspace(c)) {
				lang[i] = '\0';
				error("bad language identifier\n");
				state = ERROR;
				i = 0;
			}
			break;
		  case DONE:
			if (c == '\n') {
				lang[i] = '\0';
				*p = '\0';
				return(buf);
			}
			if (!isspace(c)) {
				error("extra characters after '\"'\n");
				state = ERROR;
			}
			break;
		  case ERROR:
			if (c == '\n')
				state = ERR1;
			break;
		  case ERR1:
			state = (c == '\n' ? NEWLINE : ERROR);
			break;
		}
	}
	if (state != NEWLINE)
		error("unexpected EOF\n");
	return(NULL);
}


static struct indx_ent {
	word32	crc;
	long	offset;
} *indx_tbl = NULL;

static int max_msgs = 0;
static int nmsg = 0;

static FILE *langf;

static struct {
	long lang_fsize;	/* size of language.txt */
	char lang[16];		/* language identifier */
	int nmsg;		/* number of messages */
} indx_hdr;


static int make_indexfile(char *);
word32 crcupdate(byte, word32);
void init_crc();

/*
 * uses 24-bit CRC function from armor.c
 */
static word32
message_crc(char *s)
{
	word32 crc = 0;

	while (*s)
		crc = crcupdate(*s++, crc);
	return(crc);
}

/*
 * lookup file offset in indx_tbl
 */
static long
lookup_offset(word32 crc)
{
	int i;
	
	for (i = 0; i < nmsg; ++i)
		if (indx_tbl[i].crc == crc)
			return(indx_tbl[i].offset);
	return(-1);
}



#ifndef LANGTOOL
static void init_lang();

/*
 * return foreign translation of s
 */
char *
PSTR (char *s)
{
	long filepos;

	if (subtitles_available == 0)
		init_lang();
	if (subtitles_available < 0)
		return(s);

	filepos = lookup_offset(message_crc(s));
	if (filepos == -1) {
		return(s);
	} else {
		fseek(langf, filepos, SEEK_SET);
		readstr(langf, strbuf, 1);
	}

	if (strbuf[0] == '\0')
		return(s);

	for (s = strbuf; *s; ++s)
		*s = EXT_C(*s);
	return(strbuf);
}

/*
 * initialize the index table: read it from language.idx or create
 * a new one and write it to the index file. A new index file is
 * created if the language set in config.pgp doesn't match the one
 * in language.idx or if the size of language.txt has changed.
 */
static void
init_lang()
{
	char indexfile[MAX_PATH];
	char subtitles_file[MAX_PATH];
	FILE *indexf;

	if (strcmp(language, "en") == 0) {
		subtitles_available = -1;
		return;		/* use default messages */
	}

	buildfilename (subtitles_file, langfile);
	if ((langf = fopen(subtitles_file, "rb")) == NULL) {
		subtitles_available = -1;
		return;
	}
	init_crc();
	if ((strbuf = (char *) malloc(STRBUFSIZE)) == NULL) {
		fprintf(stderr, "Not enough memory for foreign subtitles\n");
		fclose(langf);
		subtitles_available = -1;
		return;
	}
	buildfilename(indexfile, LANG_INDEXFILE);
	if ((indexf = fopen(indexfile, "rb")) != NULL) {
		if (fread(&indx_hdr, sizeof(indx_hdr), 1, indexf) == 1 &&
			indx_hdr.lang_fsize == fsize(langf) &&
			strcmp(indx_hdr.lang, language) == 0)
		{
			nmsg = indx_hdr.nmsg;
			indx_tbl = (struct indx_ent *) malloc(nmsg * sizeof(struct indx_ent));
			if (indx_tbl == NULL) {
				fprintf(stderr, "Not enough memory for foreign subtitles\n");
				fclose(indexf);
				fclose(langf);
				subtitles_available = -1;
				return;
			}
			if (fread(indx_tbl, sizeof(struct indx_ent), nmsg, indexf) != nmsg)
			{
				free(indx_tbl);	/* create a new one */
				indx_tbl = NULL;
			}
		}
		fclose(indexf);
	}
	if (indx_tbl == NULL && make_indexfile(indexfile) < 0) {
		fclose(langf);
		subtitles_available = -1;
	} else {
		subtitles_available = 1;
	}
}
#endif /* !LANGTOOL */


/*
 * build the index table in memory, and if indexfile is not NULL,
 * write it to this file
 */
static int
make_indexfile(char *indexfile)
{
	FILE *indexf;
	long filepos;
	int total_msgs = 0;
	char *res;

	rewind(langf);
	indx_hdr.lang_fsize = fsize(langf);
	strncpy(indx_hdr.lang, language, 15);
	init_crc();
	line = 1;
	nmsg = 0;
	while (readstr(langf, strbuf, 0)) {
		if (nmsg == max_msgs) {
			if (max_msgs) {
				max_msgs *= 2;
				indx_tbl = (struct indx_ent *) realloc(indx_tbl, max_msgs *
							sizeof(struct indx_ent));
			} else {
				max_msgs = 400;
				indx_tbl = (struct indx_ent *) malloc(max_msgs *
							sizeof(struct indx_ent));
			}
			if (indx_tbl == NULL) {
				fprintf(stderr, "Not enough memory for foreign subtitles\n");
				return(-1);
			}
		}
		++total_msgs;
		indx_tbl[nmsg].crc = message_crc(strbuf);
		if (lookup_offset(indx_tbl[nmsg].crc) != -1)
			error("message CRC not unique.\n");
		do {
			filepos = ftell(langf);
			res = readstr (langf, strbuf, 1);		/* Abort if find newline first */
			if (*language == '\0')		/* use first language found */
				strcpy(language, lang);
		} while (res && strbuf[0] != '\0' && strcmp(language, lang) != 0);

		if (res == NULL)
			break;
		if (strbuf[0] == '\0')	/* No translation */
			continue;

		indx_tbl[nmsg].offset = filepos;
		++nmsg;
		do
			res = readstr (langf, strbuf, 1);		/* Abort if find newline first */
		while (res && strbuf[0] != '\0');
	}
	line = 0;
	indx_hdr.nmsg = nmsg;
	if (verbose)
		fprintf(stderr, "%s: %d messages, %d translations for language \"%s\"\n",
				langfile, total_msgs, nmsg, language);
	if (nmsg == 0) {
		fprintf(stderr, "No translations available for language \"%s\"\n\n",
				language);
		return(-1);
	}

	if (indexfile) {
		if ((indexf = fopen(indexfile, "wb")) == NULL)
			fprintf(stderr, "Cannot create %s\n", indexfile);
		else {
			fwrite(&indx_hdr, 1, sizeof(indx_hdr), indexf);
			fwrite(indx_tbl, sizeof(struct indx_ent), nmsg, indexf);
			if (ferror(indexf) || fclose(indexf))
				fprintf(stderr, "error writing %s\n", indexfile);
		}
	}
	return(0);
}

static void
error(char *s)
{
	++errcount;
	if (langfile[0])
		fprintf(stderr, "%s:", langfile);
	if (line)
		fprintf(stderr, "%d:", line);
	fprintf(stderr, " %s", s);
}

#ifdef LANGTOOL
/*
 * language string tool for manipulating language files
 * link with CRC routines from armor.c
 */

#define CMD_EXTRACT	1
#define CMD_CHECK	2
#define CMD_MERGE	3

extern char *optarg;
extern int optind;

main(int argc, char **argv)
{
	int opt, cmd = 0, rc = 0;
	char *langIDs[16];
	char *outfile = NULL;

	init_crc();
	if ((strbuf = (char *) malloc(STRBUFSIZE)) == NULL) {
		perror(argv[0]);
		exit(1);
	}
	while ((opt = getopt(argc, argv, "cxmo:")) != EOF) {
		switch (opt) {
		  case 'c': cmd = CMD_CHECK; break;
		  case 'x': cmd = CMD_EXTRACT; break;
		  case 'm': cmd = CMD_MERGE; break;
		  case 'o': outfile = optarg; break;
		  default: usage();
		}
	}
	argc -= optind; argv += optind;
	switch (cmd) {
		case CMD_EXTRACT:
			if (argc < 2)
				usage();
			rc = extract(argv[0], outfile, &argv[1]);
			break;
		case CMD_MERGE:
			if (argc < 2)
				usage();
			rc = merge(argv[0], argv[1], outfile, argv[2]);
			break;
		case CMD_CHECK:
			verbose = 1;
			if (argc == 0)
				checkfile("language.txt");
			else
				while (--argc >= 0)
					checkfile(*argv++);
			break;
		default: usage();
	}
	exit(rc);
}

usage()
{
	fprintf(stderr, "usage: langtool -[x|c|m] [-o outputfile] ...\n\n\
To extract one or more languages from a merged file:\n\
	langtool -x [-o outputfile] file langID...\n\n\
To check a language file for syntax errors:\n\
	langtool -c file...\n\n\
To merge language \"lang\" from lang_file with source_file:\n\
	langtool -m [-o outputfile] source_file lang_file [lang]\n");
	exit(1);
}

merge(char *base_file, char *lang_file, char *outfile, char *langID)
{
	FILE *fp, *outf;
	long fpos = 0, filepos;
	int newmsgs = 0;

	if ((langf = fopen(lang_file, "r")) == NULL) {
		perror(lang_file);
		return -1;
	}
	strcpy(langfile, lang_file);
	if (langID)
		strcpy(language, langID);
	else
		language[0] = '\0';	/* use first language found */

	errcount = 0;
	make_indexfile(NULL);
	if (errcount)
		return -1;

	langfile[0] = '\0';	/* don't print filename in error msgs */

	if ((fp = fopen(base_file, "r")) == NULL) {
		perror(base_file);
		return -1;
	}
	if (outfile == NULL)
		outf = stdout;
	else {
		if ((outf = fopen(outfile, "w")) == NULL) {
			perror(outfile);
			return(-1);
		}
	}

	while (readstr(fp, strbuf, 0)) {
		copypos(fp, outf, fpos);
		fpos = ftell(fp);

		filepos = lookup_offset(message_crc(strbuf));
		if (filepos == -1) {
			fprintf(outf, "No translation\n");
			++newmsgs;
		} else {
			fseek(langf, filepos, SEEK_SET);
			readstr(langf, strbuf, 1);
			copypos(langf, outf, filepos);
		}

		while (readstr(fp, strbuf, 1))
			if (*strbuf == '\0')
				break;
	}
	copypos(fp, outf, fpos);
	fflush(outf);
	if (ferror(outf)) {
		perror(outfile);
		return -1;
	}
	if (newmsgs)
		fprintf(stderr, "%d untranslated messages\n", newmsgs);
	return errcount;
}

extract(char *infile, char *outfile, char **langIDs)
{
	FILE *fp, *outf;
	long fpos = 0;
	char **langID;

	if ((fp = fopen(infile, "r")) == NULL) {
		perror(infile);
		return -1;
	}
	if (outfile == NULL) {
		outf = stdout;
	} else {
		if ((outf = fopen(outfile, "w")) == NULL) {
			perror(outfile);
			fclose(fp);
			return(-1);
		}
	}

	while (readstr(fp, strbuf, 0)) {
		copypos(fp, outf, fpos);
		fpos = ftell(fp);
		while (readstr(fp, strbuf, 1)) {
			if (*strbuf == '\0')
				break;
			for (langID = langIDs; *langID; ++langID) {
				if (strcmp(lang, *langID) == 0)
					copypos(fp, outf, fpos);
			}
			fpos = ftell(fp);
		}
	}
	copypos(fp, outf, fpos);
	fflush(outf);
	if (ferror(outf)) {
		perror(outfile);
		return -1;
	}
	return 0;
}

checkfile(char *name)
{
	if ((langf = fopen(name, "rb")) == NULL) {
		perror(name);
		return -1;
	}
	strcpy(langfile, name);
	language[0] = '\0';	/* count messages for first language */
	errcount = 0;
	make_indexfile(NULL);
	fclose(langf);
	return errcount;
}

copypos(FILE *f, FILE *g, long pos)
{
	long size;
	size = ftell(f) - pos;
	fseek(f, pos, SEEK_SET);
	copyfile(f, g, size);
}

copyfile(FILE *f, FILE *g, long n)
{
	int c;

	while (--n >= 0 && (c = getc(f)) != EOF)
		putc(c, g);
}

long
fsize(FILE *f)
{
	long len, pos;
	pos = ftell(f);
	fseek(f, 0L, SEEK_END);
	len = ftell(f);
	fseek(f, pos, SEEK_SET);
	return len;
}
#endif /* LANGTOOL */

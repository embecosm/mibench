/* $Header: /home/mguthaus/.cvsroot/mibench/consumer/tiff-v3.5.4/tools/fax2tiff.c,v 1.1.1.1 2000/11/06 19:52:34 mguthaus Exp $ */

/*
 * Copyright (c) 1990-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/* 
 * Convert a CCITT Group 3 FAX file to TIFF Group 3 format.
 */
#include <stdio.h>
#include <stdlib.h>		/* should have atof & getopt */
#include "tiffiop.h"

#ifndef BINMODE
#define	BINMODE
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE	1
#endif

TIFF	*faxTIFF;
#define XSIZE		1728
char	rowbuf[TIFFhowmany(XSIZE,8)];
char	refbuf[TIFFhowmany(XSIZE,8)];

int	verbose;
int	stretch;
uint16	badfaxrun;
uint32	badfaxlines;

int	copyFaxFile(TIFF* tifin, TIFF* tifout);
static	void usage(void);

static tsize_t
DummyReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	(void) fd; (void) buf; (void) size;
	return (0);
}

static tsize_t
DummyWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	(void) fd; (void) buf; (void) size;
	return (size);
}

int
main(int argc, char* argv[])
{
	FILE *in;
	TIFF *out = NULL;
	TIFFErrorHandler whandler;
	int compression = COMPRESSION_CCITTFAX3;
	int fillorder = FILLORDER_LSB2MSB;
	uint32 group3options = GROUP3OPT_FILLBITS|GROUP3OPT_2DENCODING;
	int photometric = PHOTOMETRIC_MINISWHITE;
	int mode = FAXMODE_CLASSF;
	int rows;
	int c;
	int pn, npages;
	extern int optind;
	extern char* optarg;

	/* smuggle a descriptor out of the library */
	faxTIFF = TIFFClientOpen("(FakeInput)", "w", (thandle_t) -1,
				 DummyReadProc, DummyWriteProc,
				 NULL, NULL, NULL, NULL, NULL);
	if (faxTIFF == NULL)
		return (EXIT_FAILURE);
	faxTIFF->tif_mode = O_RDONLY;

	TIFFSetField(faxTIFF, TIFFTAG_IMAGEWIDTH,	XSIZE);
	TIFFSetField(faxTIFF, TIFFTAG_SAMPLESPERPIXEL,	1);
	TIFFSetField(faxTIFF, TIFFTAG_BITSPERSAMPLE,	1);
	TIFFSetField(faxTIFF, TIFFTAG_FILLORDER,	FILLORDER_LSB2MSB);
	TIFFSetField(faxTIFF, TIFFTAG_PLANARCONFIG,	PLANARCONFIG_CONTIG);
	TIFFSetField(faxTIFF, TIFFTAG_PHOTOMETRIC,	PHOTOMETRIC_MINISWHITE);
	TIFFSetField(faxTIFF, TIFFTAG_YRESOLUTION,	196.);
	TIFFSetField(faxTIFF, TIFFTAG_RESOLUTIONUNIT,	RESUNIT_INCH);
	/* NB: this is normally setup when a directory is read */
	faxTIFF->tif_scanlinesize = TIFFScanlineSize(faxTIFF);

	while ((c = getopt(argc, argv, "R:o:2BLMW14cflmpsvwz")) != -1)
		switch (c) {
			/* input-related options */
		case '2':		/* input is 2d-encoded */
			TIFFSetField(faxTIFF,
			    TIFFTAG_GROUP3OPTIONS, GROUP3OPT_2DENCODING);
			break;
		case 'B':		/* input has 0 mean black */
			TIFFSetField(faxTIFF,
			    TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
			break;
		case 'L':		/* input has lsb-to-msb fillorder */
			TIFFSetField(faxTIFF,
			    TIFFTAG_FILLORDER, FILLORDER_LSB2MSB);
			break;
		case 'M':		/* input has msb-to-lsb fillorder */
			TIFFSetField(faxTIFF,
			    TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
			break;
		case 'R':		/* input resolution */
			TIFFSetField(faxTIFF,
			    TIFFTAG_YRESOLUTION, atof(optarg));
			break;
		case 'W':		/* input has 0 mean white */
			TIFFSetField(faxTIFF,
			    TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
			break;

			/* output-related options */
		case '1':		/* generate 1d-encoded output */
			group3options &= ~GROUP3OPT_2DENCODING;
			break;
		case '4':		/* generate g4-encoded output */
			compression = COMPRESSION_CCITTFAX4;
			break;
		case 'c':		/* generate "classic" g3 format */
			mode = FAXMODE_CLASSIC;
			break;
		case 'f':		/* generate Class F format */
			mode = FAXMODE_CLASSF;
			break;
		case 'm':		/* output's fillorder is msb-to-lsb */
			fillorder = FILLORDER_MSB2LSB;
			break;
		case 'o':
			out = TIFFOpen(optarg, "w");
			if (out == NULL)
			    return EXIT_FAILURE;
			break;
		case 'p':		/* zero pad output scanline EOLs */
			group3options &= ~GROUP3OPT_FILLBITS;
			break;
		case 's':		/* stretch image by dup'ng scanlines */
			stretch = 1;
			break;
		case 'w':		/* undocumented -- for testing */
			photometric = PHOTOMETRIC_MINISBLACK;
			break;

		case 'z':		/* undocumented -- for testing */
			compression = COMPRESSION_LZW;
			break;

		case 'v':		/* -v for info */
			verbose++;
			break;
		case '?':
			usage();
			/*NOTREACHED*/
		}
	if (out == NULL) {
		out = TIFFOpen("fax.tif", "w");
		if (out == NULL)
			return (EXIT_FAILURE);
	}
	faxTIFF->tif_readproc = out->tif_readproc;	/* XXX */
	faxTIFF->tif_writeproc = out->tif_writeproc;	/* XXX */
	faxTIFF->tif_seekproc = out->tif_seekproc;	/* XXX */
	faxTIFF->tif_closeproc = out->tif_closeproc;	/* XXX */
	faxTIFF->tif_sizeproc = out->tif_sizeproc;	/* XXX */
	faxTIFF->tif_mapproc = out->tif_mapproc;	/* XXX */
	faxTIFF->tif_unmapproc = out->tif_unmapproc;	/* XXX */

	npages = argc - optind;
	if (npages < 1)
		usage();

	/* NB: this must be done after directory info is setup */
	TIFFSetField(faxTIFF, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX3);
	for (pn = 0; optind < argc; pn++, optind++) {
		in = fopen(argv[optind], "r" BINMODE);
		if (in == NULL) {
			fprintf(stderr,
			    "%s: %s: Can not open\n", argv[0], argv[optind]);
			continue;
		}
		faxTIFF->tif_fd = fileno(in);
		faxTIFF->tif_clientdata = (thandle_t) faxTIFF->tif_fd;
		faxTIFF->tif_name = argv[optind];
		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, XSIZE);
		TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 1);
		TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photometric);
		TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
		if (compression == COMPRESSION_CCITTFAX3) {
			TIFFSetField(out, TIFFTAG_GROUP3OPTIONS, group3options);
			TIFFSetField(out, TIFFTAG_FAXMODE, mode);
		}
		if (compression == COMPRESSION_CCITTFAX3 ||
		    compression == COMPRESSION_CCITTFAX4)
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, -1L);
		else
			TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
			    TIFFDefaultStripSize(out, 0));
		TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(out, TIFFTAG_FILLORDER, fillorder);
		TIFFSetField(out, TIFFTAG_SOFTWARE, "fax2tiff");
		TIFFSetField(out, TIFFTAG_XRESOLUTION, 204.0);
		if (!stretch) {
			float yres;
			TIFFGetField(faxTIFF, TIFFTAG_YRESOLUTION, &yres);
			TIFFSetField(out, TIFFTAG_YRESOLUTION, yres);
		} else
			TIFFSetField(out, TIFFTAG_YRESOLUTION, 196.);
		TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
		TIFFSetField(out, TIFFTAG_PAGENUMBER, pn+1, npages);

		if (!verbose)
		    whandler = TIFFSetWarningHandler(NULL);
		rows = copyFaxFile(faxTIFF, out);
		fclose(in);
		if (!verbose)
		    (void) TIFFSetWarningHandler(whandler);

		TIFFSetField(out, TIFFTAG_IMAGELENGTH, rows);

		if (verbose) {
			fprintf(stderr, "%s:\n", argv[optind]);
			fprintf(stderr, "%d rows in input\n", rows);
			fprintf(stderr, "%ld total bad rows\n",
			    (long) badfaxlines);
			fprintf(stderr, "%d max consecutive bad rows\n", badfaxrun);
		}
		if (compression == COMPRESSION_CCITTFAX3 &&
		    mode == FAXMODE_CLASSF) {
			TIFFSetField(out, TIFFTAG_BADFAXLINES, badfaxlines);
			TIFFSetField(out, TIFFTAG_CLEANFAXDATA, badfaxlines ?
			    CLEANFAXDATA_REGENERATED : CLEANFAXDATA_CLEAN);
			TIFFSetField(out, TIFFTAG_CONSECUTIVEBADFAXLINES, badfaxrun);
		}
		TIFFWriteDirectory(out);
	}
	TIFFClose(out);
	return (EXIT_SUCCESS);
}

int
copyFaxFile(TIFF* tifin, TIFF* tifout)
{
	uint32 row;
	uint16 badrun;
	int ok;

	tifin->tif_rawdatasize = TIFFGetFileSize(tifin);
	tifin->tif_rawdata = _TIFFmalloc(tifin->tif_rawdatasize);
	if (!ReadOK(tifin, tifin->tif_rawdata, tifin->tif_rawdatasize)) {
		TIFFError(tifin->tif_name, "%s: Read error at scanline 0");
		return (0);
	}
	tifin->tif_rawcp = tifin->tif_rawdata;
	tifin->tif_rawcc = tifin->tif_rawdatasize;

	(*tifin->tif_setupdecode)(tifin);
	(*tifin->tif_predecode)(tifin, (tsample_t) 0);
	tifin->tif_row = 0;
	badfaxlines = 0;
	badfaxrun = 0;

	_TIFFmemset(refbuf, 0, sizeof (refbuf));
	row = 0;
	badrun = 0;		/* current run of bad lines */
	while (tifin->tif_rawcc > 0) {
		ok = (*tifin->tif_decoderow)(tifin, rowbuf, sizeof (rowbuf), 0);
		if (!ok) {
			badfaxlines++;
			badrun++;
			/* regenerate line from previous good line */
			_TIFFmemcpy(rowbuf, refbuf, sizeof (rowbuf));
		} else {
			if (badrun > badfaxrun)
				badfaxrun = badrun;
			badrun = 0;
			_TIFFmemcpy(refbuf, rowbuf, sizeof (rowbuf));
		}
		tifin->tif_row++;

		if (TIFFWriteScanline(tifout, rowbuf, row, 0) < 0) {
			fprintf(stderr, "%s: Write error at row %ld.\n",
			    tifout->tif_name, (long) row);
			break;
		}
		row++;
		if (stretch) {
			if (TIFFWriteScanline(tifout, rowbuf, row, 0) < 0) {
				fprintf(stderr, "%s: Write error at row %ld.\n",
				    tifout->tif_name, (long) row);
				break;
			}
			row++;
		}
	}
	if (badrun > badfaxrun)
		badfaxrun = badrun;
	_TIFFfree(tifin->tif_rawdata);
	return (row);
}

char* stuff[] = {
"usage: fax2tiff [options] input.g3...",
"where options are:",
" -2		input data is 2d encoded",
" -B		input data has min 0 means black",
" -L		input data has LSB2MSB bit order (default)",
" -M		input data has MSB2LSB bit order",
" -W		input data has min 0 means white (default)",
" -R #		input data has # resolution (lines/inch) (default is 196)",
"",
" -o out.tif	write output to out.tif",
" -1		generate 1d-encoded output (default is G3 2d)",
" -4		generate G4-encoded output (default is G3 2D)",
" -c		generate \"classic\" TIFF format (default is TIFF/F)",
" -f		generate TIFF Class F (TIFF/F) format (default)",
" -m		output fill order is MSB2LSB (default is LSB2MSB)",
" -p		do not byte-align EOL codes in output (default is byte-align)",
" -s		stretch image by duplicating scanlines",
" -v		print information about conversion work",
NULL
};

static void
usage(void)
{
	char buf[BUFSIZ];
	int i;

	setbuf(stderr, buf);
	for (i = 0; stuff[i] != NULL; i++)
		fprintf(stderr, "%s\n", stuff[i]);
	exit(EXIT_FAILURE);
}

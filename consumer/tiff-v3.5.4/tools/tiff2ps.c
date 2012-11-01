/* $Header: /home/mguthaus/.cvsroot/mibench/consumer/tiff-v3.5.4/tools/tiff2ps.c,v 1.1.1.1 2000/11/06 19:52:44 mguthaus Exp $ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
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

#include <stdio.h>
#include <stdlib.h>			/* for atof */
#include <math.h>
#include <time.h>

#include "tiffio.h"

/*
 * NB: this code assumes uint32 works with printf's %l[ud].
 */
#ifndef TRUE
#define	TRUE	1
#define	FALSE	0
#endif

int     ascii85 = FALSE;		/* use ASCII85 encoding */
int	interpolate = TRUE;		/* interpolate level2 image */
int	level2 = FALSE;			/* generate PostScript level 2 */
int	printAll = FALSE;		/* print all images in file */
int	generateEPSF = TRUE;		/* generate Encapsulated PostScript */
int 	PSduplex = FALSE;		/* enable duplex printing */
int	PStumble = FALSE;		/* enable top edge binding */
int	PSavoiddeadzone = TRUE;		/* enable avoiding printer deadzone */
char	*filename;			/* input filename */

/*
 * ASCII85 Encoding Support.
 */
unsigned char ascii85buf[10];
int	ascii85count;
int	ascii85breaklen;

int	TIFF2PS(FILE*, TIFF*, float, float);
void	PSpage(FILE*, TIFF*, uint32, uint32);
void	PSColorContigPreamble(FILE*, uint32, uint32, int);
void	PSColorSeparatePreamble(FILE*, uint32, uint32, int);
void	PSDataColorContig(FILE*, TIFF*, uint32, uint32, int);
void	PSDataColorSeparate(FILE*, TIFF*, uint32, uint32, int);
void	PSDataPalette(FILE*, TIFF*, uint32, uint32);
void	PSDataBW(FILE*, TIFF*, uint32, uint32);
void	PSRawDataBW(FILE*, TIFF*, uint32, uint32);
void	Ascii85Init(void);
void	Ascii85Put(unsigned char code, FILE* fd);
void	Ascii85Flush(FILE* fd);
void    PSHead(FILE*, TIFF*, uint32, uint32, float, float, float, float);
void 	PSTail(FILE*, int);

static	void usage(int);

int
main(int argc, char* argv[])
{
	int dirnum = -1, c, np = 0;
	float pageWidth = 0;
	float pageHeight = 0;
	uint32 diroff = 0;
	extern char *optarg;
	extern int optind;
	FILE* output = stdout;

	while ((c = getopt(argc, argv, "h:w:d:o:O:aezps128DT")) != -1)
		switch (c) {
		case 'd':
			dirnum = atoi(optarg);
			break;
	        case 'D':
			PSduplex = TRUE;
			break;
		case 'T':
			PStumble = TRUE;
			break;
		case 'e':
			generateEPSF = TRUE;
			break;
		case 'h':
			pageHeight = atof(optarg);
			break;
		case 'o':
			diroff = (uint32) strtoul(optarg, NULL, 0);
			break;
		case 'O':		/* XXX too bad -o is already taken */
			output = fopen(optarg, "w");
			if (output == NULL) {
				fprintf(stderr,
				    "%s: %s: Cannot open output file.\n",
				    argv[0], optarg);
				exit(-2);
			}
			break;
		case 'a':
			printAll = TRUE;
			/* fall thru... */
		case 'p':
			generateEPSF = FALSE;
			break;
		case 's':
			printAll = FALSE;
			break;
		case 'w':
			pageWidth = atof(optarg);
			break;
		case 'z':
			PSavoiddeadzone = FALSE;
			break;
		case '1':
			level2 = FALSE;
			ascii85 = FALSE;
			break;
		case '2':
			level2 = TRUE;
			ascii85 = TRUE;			/* default to yes */
			break;
		case '8':
			ascii85 = FALSE;
			break;
		case '?':
			usage(-1);
		}
	for (; argc - optind > 0; optind++) {
		TIFF* tif = TIFFOpen(filename = argv[optind], "r");
		if (tif != NULL) {
			if (dirnum != -1 && !TIFFSetDirectory(tif, dirnum))
				return (-1);
			else if (diroff != 0 &&
			    !TIFFSetSubDirectory(tif, diroff))
				return (-1);
			np = TIFF2PS(output, tif, pageWidth, pageHeight);
			TIFFClose(tif);
		}
	}
	if (np)
	        PSTail(output, np);
	else
	        usage(-1);
	if (output != stdout)
		fclose(output);
	return (0);
}

static	uint16 samplesperpixel;
static	uint16 bitspersample;
static	uint16 planarconfiguration;
static	uint16 photometric;
static	uint16 compression;
static	uint16 extrasamples;
static	int alpha;

static int
checkImage(TIFF* tif)
{
	switch (photometric) {
	case PHOTOMETRIC_YCBCR:
		if (compression == COMPRESSION_JPEG &&
		    planarconfiguration == PLANARCONFIG_CONTIG) {
			/* can rely on libjpeg to convert to RGB */
			TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE,
				     JPEGCOLORMODE_RGB);
			photometric = PHOTOMETRIC_RGB;
		} else {
			if (level2)
				break;
			TIFFError(filename, "Can not handle image with %s",
			    "PhotometricInterpretation=YCbCr");
			return (0);
		}
		/* fall thru... */
	case PHOTOMETRIC_RGB:
		if (alpha && bitspersample != 8) {
			TIFFError(filename,
			    "Can not handle %d-bit/sample RGB image with alpha",
			    bitspersample);
			return (0);
		}
		/* fall thru... */
	case PHOTOMETRIC_SEPARATED:
	case PHOTOMETRIC_PALETTE:
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
		break;
	case PHOTOMETRIC_LOGL:
	case PHOTOMETRIC_LOGLUV:
		if (compression != COMPRESSION_SGILOG &&
		    compression != COMPRESSION_SGILOG24) {
			TIFFError(filename,
		    "Can not handle %s data with compression other than SGILog",
			    (photometric == PHOTOMETRIC_LOGL) ?
				"LogL" : "LogLuv"
			);
			return (0);
		}
		/* rely on library to convert to RGB/greyscale */
		TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_8BIT);
		photometric = (photometric == PHOTOMETRIC_LOGL) ?
		    PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;
		bitspersample = 8;
		break;
	case PHOTOMETRIC_CIELAB:
		/* fall thru... */
	default:
		TIFFError(filename,
		    "Can not handle image with PhotometricInterpretation=%d",
		    photometric);
		return (0);
	}
	switch (bitspersample) {
	case 1: case 2:
	case 4: case 8:
		break;
	default:
		TIFFError(filename, "Can not handle %d-bit/sample image",
		    bitspersample);
		return (0);
	}
	if (planarconfiguration == PLANARCONFIG_SEPARATE && extrasamples > 0)
		TIFFWarning(filename, "Ignoring extra samples");
	return (1);
}

#define PS_UNIT_SIZE	72.0
#define	PSUNITS(npix,res)	((npix) * (PS_UNIT_SIZE / (res)))

static	char RGBcolorimage[] = "\
/bwproc {\n\
    rgbproc\n\
    dup length 3 idiv string 0 3 0\n\
    5 -1 roll {\n\
	add 2 1 roll 1 sub dup 0 eq {\n\
	    pop 3 idiv\n\
	    3 -1 roll\n\
	    dup 4 -1 roll\n\
	    dup 3 1 roll\n\
	    5 -1 roll put\n\
	    1 add 3 0\n\
	} { 2 1 roll } ifelse\n\
    } forall\n\
    pop pop pop\n\
} def\n\
/colorimage where {pop} {\n\
    /colorimage {pop pop /rgbproc exch def {bwproc} image} bind def\n\
} ifelse\n\
";

/*
 * Adobe Photoshop requires a comment line of the form:
 *
 * %ImageData: <cols> <rows> <depth>  <main channels> <pad channels>
 *	<block size> <1 for binary|2 for hex> "data start"
 *
 * It is claimed to be part of some future revision of the EPS spec.
 */
static void
PhotoshopBanner(FILE* fd, uint32 w, uint32 h, int bs, int nc, char* startline)
{
	fprintf(fd, "%%ImageData: %ld %ld %d %d 0 %d 2 \"",
	    (long) w, (long) h, bitspersample, nc, bs);
	fprintf(fd, startline, nc);
	fprintf(fd, "\"\n");
}

static void
setupPageState(TIFF* tif, uint32* pw, uint32* ph, float* pprw, float* pprh)
{
	uint16 res_unit;
	float xres, yres;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, pw);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, ph);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	/*
	 * Calculate printable area.
	 */
	if (!TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres))
		xres = PS_UNIT_SIZE;
	if (!TIFFGetField(tif, TIFFTAG_YRESOLUTION, &yres))
		yres = PS_UNIT_SIZE;
	switch (res_unit) {
	case RESUNIT_CENTIMETER:
		xres *= 2.54, yres *= 2.54;
		break;
	case RESUNIT_NONE:
		xres *= PS_UNIT_SIZE, yres *= PS_UNIT_SIZE;
		break;
	}
	*pprh = PSUNITS(*ph, yres);
	*pprw = PSUNITS(*pw, xres);
}

static int
isCCITTCompression(TIFF* tif)
{
    uint16 compress;
    TIFFGetField(tif, TIFFTAG_COMPRESSION, &compress);
    return (compress == COMPRESSION_CCITTFAX3 ||
	    compress == COMPRESSION_CCITTFAX4 ||
	    compress == COMPRESSION_CCITTRLE ||
	    compress == COMPRESSION_CCITTRLEW);
}

static	tsize_t tf_bytesperrow;
static	tsize_t ps_bytesperrow;
static 	tsize_t	tf_rowsperstrip;
static	tsize_t	tf_numberstrips;
static	char *hex = "0123456789abcdef";


/* returns the sequence number of the page processed */
int
TIFF2PS(FILE* fd, TIFF* tif, float pw, float ph)
{
	uint32 w, h;
	float ox, oy, prw, prh;
	float scale;
	uint32 subfiletype;
	uint16* sampleinfo;
	static int npages = 0;

	if (!TIFFGetField(tif, TIFFTAG_XPOSITION, &ox))
		ox = 0;
	if (!TIFFGetField(tif, TIFFTAG_YPOSITION, &oy))
		oy = 0;
	setupPageState(tif, &w, &h, &prw, &prh);

	do {
	        tf_numberstrips = TIFFNumberOfStrips(tif);
		TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP,
		    &tf_rowsperstrip);
		setupPageState(tif, &w, &h, &prw, &prh);
		if (!npages)
		        PSHead(fd, tif, w, h, prw, prh, ox, oy);
		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE,
		    &bitspersample);
		TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL,
		    &samplesperpixel);
		TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG,
		    &planarconfiguration);
		TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);
		TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES,
		    &extrasamples, &sampleinfo);
		alpha = (extrasamples == 1 &&
			 sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA);
		if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric)) {
			switch (samplesperpixel - extrasamples) {
			case 1:
				if (isCCITTCompression(tif))
					photometric = PHOTOMETRIC_MINISWHITE;
				else
					photometric = PHOTOMETRIC_MINISBLACK;
				break;
			case 3:
				photometric = PHOTOMETRIC_RGB;
				break;
			}
		}
		if (checkImage(tif)) {
			tf_bytesperrow = TIFFScanlineSize(tif);
			npages++;
			fprintf(fd, "%%%%Page: %d %d\n", npages, npages);
			fprintf(fd, "gsave\n");
			fprintf(fd, "100 dict begin\n");
			if (pw != 0 && ph != 0) {
				/* NB: maintain image aspect ratio */
				scale = (pw*PS_UNIT_SIZE/prw) < (ph*PS_UNIT_SIZE/prh) ?
				    (pw*PS_UNIT_SIZE/prw) :
				    (ph*PS_UNIT_SIZE/prh);
				if (scale > 1.0)
					scale = 1.0;
				fprintf(fd, "0 %f translate\n", ph*PS_UNIT_SIZE-prh*scale);
				fprintf(fd, "%f %f scale\n", prw*scale, prh*scale);
			} else
				fprintf(fd, "%f %f scale\n", prw, prh);
			PSpage(fd, tif, w, h);
			fprintf(fd, "end\n");
			fprintf(fd, "grestore\n");
			fprintf(fd, "showpage\n");
		}
		if (generateEPSF)
			break;
		TIFFGetFieldDefaulted(tif, TIFFTAG_SUBFILETYPE, &subfiletype);
	} while (((subfiletype & FILETYPE_PAGE) || printAll) &&
	    TIFFReadDirectory(tif));

	return(npages);
}


static char DuplexPreamble[] = "\
%%BeginFeature: *Duplex True\n\
systemdict begin\n\
  /languagelevel where { pop languagelevel } { 1 } ifelse\n\
  2 ge { 1 dict dup /Duplex true put setpagedevice }\n\
  { statusdict /setduplex known { statusdict begin setduplex true end } if\n\
  } ifelse\n\
end\n\
%%EndFeature\n\
";

static char TumblePreamble[] = "\
%%BeginFeature: *Tumble True\n\
systemdict begin\n\
  /languagelevel where { pop languagelevel } { 1 } ifelse\n\
  2 ge { 1 dict dup /Tumble true put setpagedevice }\n\
  { statusdict /settumble known { statusdict begin settumble true end } if\n\
  } ifelse\n\
end\n\
%%EndFeature\n\
";

static char AvoidDeadZonePreamble[] = "\
gsave newpath clippath pathbbox grestore\n\
  4 2 roll 2 copy translate\n\
  exch 3 1 roll sub 3 1 roll sub exch\n\
  currentpagedevice /PageSize get aload pop\n\
  exch 3 1 roll div 3 1 roll div abs exch abs\n\
  2 copy gt { exch } if pop\n\
  dup 1 lt { dup scale } { pop } ifelse\n\
";

void
PSHead(FILE *fd, TIFF *tif, uint32 w, uint32 h, float pw, float ph,
	float ox, float oy)
{
        time_t t;

	(void) tif; (void) w; (void) h;
	t = time(0);
	fprintf(fd, "%%!PS-Adobe-3.0%s\n", generateEPSF ? " EPSF-3.0" : "");
	fprintf(fd, "%%%%Creator: tiff2ps\n");
	fprintf(fd, "%%%%Title: %s\n", filename);
	fprintf(fd, "%%%%CreationDate: %s", ctime(&t));
	fprintf(fd, "%%%%DocumentData: Clean7Bit\n");
	fprintf(fd, "%%%%Origin: %ld %ld\n", (long) ox, (long) oy);
	/* NB: should use PageBoundingBox */
	fprintf(fd, "%%%%BoundingBox: 0 0 %ld %ld\n",
	    (long) ceil(pw), (long) ceil(ph));
	fprintf(fd, "%%%%LanguageLevel: %d\n", level2 ? 2 : 1);
	fprintf(fd, "%%%%Pages: (atend)\n");
	fprintf(fd, "%%%%EndComments\n");
	fprintf(fd, "%%%%BeginSetup\n");
	if (PSduplex)
	        fprintf(fd, "%s", DuplexPreamble);
	if (PStumble)
	        fprintf(fd, "%s", TumblePreamble);
	if (PSavoiddeadzone && level2)
	        fprintf(fd, "%s", AvoidDeadZonePreamble);
	fprintf(fd, "%%%%EndSetup\n");
}

void
PSTail(FILE *fd, int npages)
{
	fprintf(fd, "%%%%Trailer\n");
        fprintf(fd, "%%%%Pages: %d\n", npages);
	fprintf(fd, "%%%%EOF\n");
}

static int
checkcmap(TIFF* tif, int n, uint16* r, uint16* g, uint16* b)
{
	(void) tif;
	while (n-- > 0)
		if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
			return (16);
	TIFFWarning(filename, "Assuming 8-bit colormap");
	return (8);
}

static void
PS_Lvl2colorspace(FILE* fd, TIFF* tif)
{
	uint16 *rmap, *gmap, *bmap;
	int i, num_colors;

	/*
	 * Set up PostScript Level 2 colorspace according to
	 * section 4.8 in the PostScript refenence manual.
	 */
	fputs("% PostScript Level 2 only.\n", fd);
	if (photometric != PHOTOMETRIC_PALETTE) {
		if (photometric == PHOTOMETRIC_YCBCR) {
		    /* MORE CODE HERE */
		}
		fprintf(fd, "/Device%s",
		    samplesperpixel > 2 ? "RGB" : "Gray");
		fputs(" setcolorspace\n", fd);
		return;
	}

	/*
	 * Set up an indexed/palette colorspace
	 */
	num_colors = (1 << bitspersample);
	if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap)) {
		TIFFError(filename,
			"Palette image w/o \"Colormap\" tag");
		return;
	}
	if (checkcmap(tif, num_colors, rmap, gmap, bmap) == 16) {
		/*
		 * Convert colormap to 8-bits values.
		 */
#define	CVT(x)		(((x) * 255) / ((1L<<16)-1))
		for (i = 0; i < num_colors; i++) {
			rmap[i] = CVT(rmap[i]);
			gmap[i] = CVT(gmap[i]);
			bmap[i] = CVT(bmap[i]);
		}
#undef CVT
	}
	fprintf(fd, "[ /Indexed /DeviceRGB %d", num_colors - 1);
	if (ascii85) {
		Ascii85Init();
		fputs("\n<~", fd);
		ascii85breaklen -= 2;
	} else
		fputs(" <", fd);
	for (i = 0; i < num_colors; i++) {
		if (ascii85) {
			Ascii85Put(rmap[i], fd);
			Ascii85Put(gmap[i], fd);
			Ascii85Put(bmap[i], fd);
		} else {
			fputs((i % 8) ? " " : "\n  ", fd);
			fprintf(fd, "%02x%02x%02x",
			    rmap[i], gmap[i], bmap[i]);
		}
	}
	if (ascii85)
		Ascii85Flush(fd);
	else
		fputs(">\n", fd);
	fputs("] setcolorspace\n", fd);
}

static int
PS_Lvl2ImageDict(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	int use_rawdata;
	uint32 tile_width, tile_height;
	uint16 predictor, minsamplevalue, maxsamplevalue;
	int repeat_count;
	char im_h[64], im_x[64], im_y[64];

	(void)strcpy(im_x, "0");
	(void)sprintf(im_y, "%lu", (long) h);
	(void)sprintf(im_h, "%lu", (long) h);
	tile_width = w;
	tile_height = h;
	if (TIFFIsTiled(tif)) {
		repeat_count = TIFFNumberOfTiles(tif);
		TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tile_width);
		TIFFGetField(tif, TIFFTAG_TILELENGTH, &tile_height);
		if (tile_width > w || tile_height > h ||
		    (w % tile_width) != 0 || (h % tile_height != 0)) {
			/*
			 * The tiles does not fit image width and height.
			 * Set up a clip rectangle for the image unit square.
			 */
			fputs("0 0 1 1 rectclip\n", fd);
		}
		if (tile_width < w) {
			fputs("/im_x 0 def\n", fd);
			(void)strcpy(im_x, "im_x neg");
		}
		if (tile_height < h) {
			fputs("/im_y 0 def\n", fd);
			(void)sprintf(im_y, "%lu im_y sub", (unsigned long) h);
		}
	} else {
		repeat_count = tf_numberstrips;
		tile_height = tf_rowsperstrip;
		if (tile_height > h)
			tile_height = h;
		if (repeat_count > 1) {
			fputs("/im_y 0 def\n", fd);
			fprintf(fd, "/im_h %lu def\n",
			    (unsigned long) tile_height);
			(void)strcpy(im_h, "im_h");
			(void)sprintf(im_y, "%lu im_y sub", (unsigned long) h);
		}
	}

	/*
	 * Output start of exec block
	 */
	fputs("{ % exec\n", fd);

	if (repeat_count > 1)
		fprintf(fd, "%d { %% repeat\n", repeat_count);

	/*
	 * Output filter options and image dictionary.
	 */
	if (ascii85)
		fputs(" /im_stream currentfile /ASCII85Decode filter def\n",
		    fd);
	fputs(" <<\n", fd);
	fputs("  /ImageType 1\n", fd);
	fprintf(fd, "  /Width %lu\n", (unsigned long) tile_width);
	fprintf(fd, "  /Height %lu\n", (unsigned long) tile_height);
	if (planarconfiguration == PLANARCONFIG_SEPARATE && samplesperpixel > 1)
		fputs("  /MultipleDataSources true\n", fd);
	fprintf(fd, "  /ImageMatrix [ %lu 0 0 %ld %s %s ]\n",
	    (unsigned long) w, - (long)h, im_x, im_y);
	fprintf(fd, "  /BitsPerComponent %d\n", bitspersample);
	fprintf(fd, "  /Interpolate %s\n", interpolate ? "true" : "false");

	switch (samplesperpixel) {
	case 1:
		switch (photometric) {
		case PHOTOMETRIC_MINISBLACK:
			fputs("  /Decode [0 1]\n", fd);
			break;
		case PHOTOMETRIC_MINISWHITE:
			switch (compression) {
			case COMPRESSION_CCITTRLE:
			case COMPRESSION_CCITTRLEW:
			case COMPRESSION_CCITTFAX3:
			case COMPRESSION_CCITTFAX4:
				/*
				 * Manage inverting with /Blackis1 flag
				 * since there migth be uncompressed parts
				 */
				fputs("  /Decode [0 1]\n", fd);
				break;
			default:
				/*
				 * ERROR...
				 */
				fputs("  /Decode [1 0]\n", fd);
				break;
			}
			break;
		case PHOTOMETRIC_PALETTE:
			TIFFGetFieldDefaulted(tif, TIFFTAG_MINSAMPLEVALUE,
			    &minsamplevalue);
			TIFFGetFieldDefaulted(tif, TIFFTAG_MAXSAMPLEVALUE,
			    &maxsamplevalue);
			fprintf(fd, "  /Decode [%u %u]\n",
				    minsamplevalue, maxsamplevalue);
			break;
		default:
			/*
			 * ERROR ?
			 */
			fputs("  /Decode [0 1]\n", fd);
			break;
		}
		break;
	case 3:
		switch (photometric) {
		case PHOTOMETRIC_RGB:
			fputs("  /Decode [0 1 0 1 0 1]\n", fd);
			break;
		case PHOTOMETRIC_MINISWHITE:
		case PHOTOMETRIC_MINISBLACK:
		default:
			/*
			 * ERROR??
			 */
			fputs("  /Decode [0 1 0 1 0 1]\n", fd);
			break;
		}
		break;
	case 4:
		/*
		 * ERROR??
		 */
		fputs("  /Decode [0 1 0 1 0 1 0 1]\n", fd);
		break;
	}
	fputs("  /DataSource", fd);
	if (planarconfiguration == PLANARCONFIG_SEPARATE &&
	    samplesperpixel > 1)
		fputs(" [", fd);
	if (ascii85)
		fputs(" im_stream", fd);
	else
		fputs(" currentfile /ASCIIHexDecode filter", fd);

	use_rawdata = TRUE;
	switch (compression) {
	case COMPRESSION_NONE:		/* 1: uncompressed */
		break;
	case COMPRESSION_CCITTRLE:	/* 2: CCITT modified Huffman RLE */
	case COMPRESSION_CCITTRLEW:	/* 32771: #1 w/ word alignment */
	case COMPRESSION_CCITTFAX3:	/* 3: CCITT Group 3 fax encoding */
	case COMPRESSION_CCITTFAX4:	/* 4: CCITT Group 4 fax encoding */
		fputs("\n\t<<\n", fd);
		if (compression == COMPRESSION_CCITTFAX3) {
			uint32 g3_options;

			fputs("\t /EndOfLine true\n", fd);
			fputs("\t /EndOfBlock false\n", fd);
			if (!TIFFGetField(tif, TIFFTAG_GROUP3OPTIONS,
					    &g3_options))
				g3_options = 0;
			if (g3_options & GROUP3OPT_2DENCODING)
				fprintf(fd, "\t /K %s\n", im_h);
			if (g3_options & GROUP3OPT_UNCOMPRESSED)
				fputs("\t /Uncompressed true\n", fd);
			if (g3_options & GROUP3OPT_FILLBITS)
				fputs("\t /EncodedByteAlign true\n", fd);
		}
		if (compression == COMPRESSION_CCITTFAX4) {
			uint32 g4_options;

			fputs("\t /K -1\n", fd);
			TIFFGetFieldDefaulted(tif, TIFFTAG_GROUP4OPTIONS,
					       &g4_options);
			if (g4_options & GROUP4OPT_UNCOMPRESSED)
				fputs("\t /Uncompressed true\n", fd);
		}
		if (!(tile_width == w && w == 1728U))
			fprintf(fd, "\t /Columns %lu\n",
			    (unsigned long) tile_width);
		fprintf(fd, "\t /Rows %s\n", im_h);
		if (compression == COMPRESSION_CCITTRLE ||
		    compression == COMPRESSION_CCITTRLEW) {
			fputs("\t /EncodedByteAlign true\n", fd);
			fputs("\t /EndOfBlock false\n", fd);
		}
		if (photometric == PHOTOMETRIC_MINISBLACK)
			fputs("\t /BlackIs1 true\n", fd);
		fprintf(fd, "\t>> /CCITTFaxDecode filter");
		break;
	case COMPRESSION_LZW:	/* 5: Lempel-Ziv  & Welch */
		TIFFGetFieldDefaulted(tif, TIFFTAG_PREDICTOR, &predictor);
		if (predictor == 2) {
			fputs("\n\t<<\n", fd);
			fprintf(fd, "\t /Predictor %u\n", predictor);
			fprintf(fd, "\t /Columns %lu\n",
			    (unsigned long) tile_width);
			fprintf(fd, "\t /Colors %u\n", samplesperpixel);
			fprintf(fd, "\t /BitsPerComponent %u\n",
			    bitspersample);
			fputs("\t>>", fd);
		}
		fputs(" /LZWDecode filter", fd);
		break;
	case COMPRESSION_PACKBITS:	/* 32773: Macintosh RLE */
		fputs(" /RunLengthDecode filter", fd);
		use_rawdata = TRUE;
	    break;
	case COMPRESSION_OJPEG:		/* 6: !6.0 JPEG */
	case COMPRESSION_JPEG:		/* 7: %JPEG DCT compression */
#ifdef notdef
		/*
		 * Code not tested yet
		 */
		fputs(" /DCTDecode filter", fd);
		use_rawdata = TRUE;
#else
		use_rawdata = FALSE;
#endif
		break;
	case COMPRESSION_NEXT:		/* 32766: NeXT 2-bit RLE */
	case COMPRESSION_THUNDERSCAN:	/* 32809: ThunderScan RLE */
	case COMPRESSION_PIXARFILM:	/* 32908: Pixar companded 10bit LZW */
	case COMPRESSION_DEFLATE:	/* 32946: Deflate compression */
	case COMPRESSION_JBIG:		/* 34661: ISO JBIG */
		use_rawdata = FALSE;
		break;
	case COMPRESSION_SGILOG:	/* 34676: SGI LogL or LogLuv */
	case COMPRESSION_SGILOG24:	/* 34677: SGI 24-bit LogLuv */
		use_rawdata = FALSE;
		break;
	default:
		/*
		 * ERROR...
		 */
		use_rawdata = FALSE;
		break;
	}
	if (planarconfiguration == PLANARCONFIG_SEPARATE &&
	    samplesperpixel > 1) {
		uint16 i;

		/*
		 * NOTE: This code does not work yet...
		 */
		for (i = 1; i < samplesperpixel; i++)
			fputs(" dup", fd);
		fputs(" ]", fd);
	}
	fputs("\n >> image\n", fd);
	if (ascii85)
		fputs(" im_stream flushfile\n", fd);
	if (repeat_count > 1) {
		if (tile_width < w) {
			fprintf(fd, " /im_x im_x %lu add def\n",
			    (unsigned long) tile_width);
			if (tile_height < h) {
				fprintf(fd, " im_x %lu ge {\n",
				    (unsigned long) w);
				fputs("  /im_x 0 def\n", fd);
				fprintf(fd, " /im_y im_y %lu add def\n",
				    (unsigned long) tile_height);
				fputs(" } if\n", fd);
			}
		}
		if (tile_height < h) {
			if (tile_width >= w) {
				fprintf(fd, " /im_y im_y %lu add def\n",
				    (unsigned long) tile_height);
				if (!TIFFIsTiled(tif)) {
					fprintf(fd, " /im_h %lu im_y sub",
					    (unsigned long) h);
					fprintf(fd, " dup %lu gt { pop",
					    (unsigned long) tile_height);
					fprintf(fd, " %lu } if def\n",
					    (unsigned long) tile_height);
				}
			}
		}
		fputs("} repeat\n", fd);
	}
	/*
	 * End of exec function
	 */
	fputs("}\n", fd);

	return(use_rawdata);
}

int
PS_Lvl2page(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	uint16 fillorder;
	int use_rawdata, tiled_image, breaklen;
	uint32 chunk_no, num_chunks, *bc;
	unsigned char *buf_data, *cp;
	tsize_t chunk_size, byte_count;

	PS_Lvl2colorspace(fd, tif);
	use_rawdata = PS_Lvl2ImageDict(fd, tif, w, h);

	fputs("%%BeginData:\n", fd);
	fputs("exec\n", fd);

	tiled_image = TIFFIsTiled(tif);
	if (tiled_image) {
		num_chunks = TIFFNumberOfTiles(tif);
		TIFFGetField(tif, TIFFTAG_TILEBYTECOUNTS, &bc);
	} else {
		num_chunks = TIFFNumberOfStrips(tif);
		TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);
	}

	if (use_rawdata) {
		chunk_size = bc[0];
		for (chunk_no = 1; chunk_no < num_chunks; chunk_no++)
			if (bc[chunk_no] > chunk_size)
				chunk_size = bc[chunk_no];
	} else {
		if (tiled_image)
			chunk_size = TIFFTileSize(tif);
		else
			chunk_size = TIFFStripSize(tif);
	}
	buf_data = (unsigned char *)_TIFFmalloc(chunk_size);
	if (!buf_data) {
		TIFFError(filename, "Can't alloc %u bytes for %s.",
			chunk_size, tiled_image ? "tiles" : "strips");
		return(FALSE);
	}

	TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &fillorder);
	for (chunk_no = 0; chunk_no < num_chunks; chunk_no++) {
		if (ascii85)
			Ascii85Init();
		else
			breaklen = 36;
		if (use_rawdata) {
			if (tiled_image)
				byte_count = TIFFReadRawTile(tif, chunk_no,
						  buf_data, chunk_size);
			else
				byte_count = TIFFReadRawStrip(tif, chunk_no,
						  buf_data, chunk_size);
			if (fillorder == FILLORDER_LSB2MSB)
			    TIFFReverseBits(buf_data, byte_count);
		} else {
			if (tiled_image)
				byte_count = TIFFReadEncodedTile(tif,
						chunk_no, buf_data,
						chunk_size);
			else
				byte_count = TIFFReadEncodedStrip(tif,
						chunk_no, buf_data,
						chunk_size);
		}
		if (byte_count < 0) {
			TIFFError(filename, "Can't read %s %d.",
				tiled_image ? "tile" : "strip", chunk_no);
			if (ascii85)
				Ascii85Put('\0', fd);
		}
		for (cp = buf_data; byte_count > 0; byte_count--) {
			if (ascii85)
				Ascii85Put(*cp++, fd);
			else {
				if (--breaklen <= 0) {
					putc('\n', fd);
					breaklen = 36;
				}
				putc(hex[((*cp)>>4)&0xf], fd);
				putc(hex[(*cp)&0xf], fd);
				cp++;
			}
		}
		if (ascii85)
			Ascii85Flush(fd);
		else
			putc('\n', fd);
	}
	_TIFFfree(buf_data);
	fputs("%%EndData\n", fd);
	return(TRUE);
}

void
PSpage(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	if (level2 && PS_Lvl2page(fd, tif, w, h))
		return;
	ps_bytesperrow = tf_bytesperrow;
	switch (photometric) {
	case PHOTOMETRIC_RGB:
		if (planarconfiguration == PLANARCONFIG_CONTIG) {
			fprintf(fd, "%s", RGBcolorimage);
			PSColorContigPreamble(fd, w, h, 3);
			PSDataColorContig(fd, tif, w, h, 3);
		} else {
			PSColorSeparatePreamble(fd, w, h, 3);
			PSDataColorSeparate(fd, tif, w, h, 3);
		}
		break;
	case PHOTOMETRIC_SEPARATED:
		/* XXX should emit CMYKcolorimage */
		if (planarconfiguration == PLANARCONFIG_CONTIG) {
			PSColorContigPreamble(fd, w, h, 4);
			PSDataColorContig(fd, tif, w, h, 4);
		} else {
			PSColorSeparatePreamble(fd, w, h, 4);
			PSDataColorSeparate(fd, tif, w, h, 4);
		}
		break;
	case PHOTOMETRIC_PALETTE:
		fprintf(fd, "%s", RGBcolorimage);
		PhotoshopBanner(fd, w, h, 1, 3, "false 3 colorimage");
		fprintf(fd, "/scanLine %ld string def\n",
		    (long) ps_bytesperrow);
		fprintf(fd, "%lu %lu 8\n",
		    (unsigned long) w, (unsigned long) h);
		fprintf(fd, "[%lu 0 0 -%lu 0 %lu]\n",
		    (unsigned long) w, (unsigned long) h, (unsigned long) h);
		fprintf(fd, "{currentfile scanLine readhexstring pop} bind\n");
		fprintf(fd, "false 3 colorimage\n");
		PSDataPalette(fd, tif, w, h);
		break;
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
		PhotoshopBanner(fd, w, h, 1, 1, "image");
		fprintf(fd, "/scanLine %ld string def\n",
		    (long) ps_bytesperrow);
		fprintf(fd, "%lu %lu %d\n",
		    (unsigned long) w, (unsigned long) h, bitspersample);
		fprintf(fd, "[%lu 0 0 -%lu 0 %lu]\n",
		    (unsigned long) w, (unsigned long) h, (unsigned long) h);
		fprintf(fd,
		    "{currentfile scanLine readhexstring pop} bind\n");
		fprintf(fd, "image\n");
		PSDataBW(fd, tif, w, h);
		break;
	}
	putc('\n', fd);
}

void
PSColorContigPreamble(FILE* fd, uint32 w, uint32 h, int nc)
{
	ps_bytesperrow = nc * (tf_bytesperrow / samplesperpixel);
	PhotoshopBanner(fd, w, h, 1, nc, "false %d colorimage");
	fprintf(fd, "/line %ld string def\n", (long) ps_bytesperrow);
	fprintf(fd, "%lu %lu %d\n",
	    (unsigned long) w, (unsigned long) h, bitspersample);
	fprintf(fd, "[%lu 0 0 -%lu 0 %lu]\n",
	    (unsigned long) w, (unsigned long) h, (unsigned long) h);
	fprintf(fd, "{currentfile line readhexstring pop} bind\n");
	fprintf(fd, "false %d colorimage\n", nc);
}

void
PSColorSeparatePreamble(FILE* fd, uint32 w, uint32 h, int nc)
{
	int i;

	PhotoshopBanner(fd, w, h, ps_bytesperrow, nc, "true %d colorimage");
	for (i = 0; i < nc; i++)
		fprintf(fd, "/line%d %ld string def\n",
		    i, (long) ps_bytesperrow);
	fprintf(fd, "%lu %lu %d\n",
	    (unsigned long) w, (unsigned long) h, bitspersample);
	fprintf(fd, "[%lu 0 0 -%lu 0 %lu] \n",
	    (unsigned long) w, (unsigned long) h, (unsigned long) h);
	for (i = 0; i < nc; i++)
		fprintf(fd, "{currentfile line%d readhexstring pop}bind\n", i);
	fprintf(fd, "true %d colorimage\n", nc);
}

#define MAXLINE		36
#define	DOBREAK(len, howmany, fd) \
	if (((len) -= (howmany)) <= 0) {	\
		putc('\n', fd);			\
		(len) = MAXLINE-(howmany);	\
	}
#define	PUTHEX(c,fd)	putc(hex[((c)>>4)&0xf],fd); putc(hex[(c)&0xf],fd)

void
PSDataColorContig(FILE* fd, TIFF* tif, uint32 w, uint32 h, int nc)
{
	uint32 row;
	int breaklen = MAXLINE, cc, es = samplesperpixel - nc;
	unsigned char *tf_buf;
	unsigned char *cp, c;

	(void) w;
	tf_buf = (unsigned char *) _TIFFmalloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		cp = tf_buf;
		if (alpha) {
			int adjust;
			cc = 0;
			for (; cc < tf_bytesperrow; cc += samplesperpixel) {
				DOBREAK(breaklen, nc, fd);
				/*
				 * For images with alpha, matte against
				 * a white background; i.e.
				 *    Cback * (1 - Aimage)
				 * where Cback = 1.
				 */
				adjust = 255 - cp[nc];
				switch (nc) {
				case 4: c = *cp++ + adjust; PUTHEX(c,fd);
				case 3: c = *cp++ + adjust; PUTHEX(c,fd);
				case 2: c = *cp++ + adjust; PUTHEX(c,fd);
				case 1: c = *cp++ + adjust; PUTHEX(c,fd);
				}
				cp += es;
			}
		} else {
			cc = 0;
			for (; cc < tf_bytesperrow; cc += samplesperpixel) {
				DOBREAK(breaklen, nc, fd);
				switch (nc) {
				case 4: c = *cp++; PUTHEX(c,fd);
				case 3: c = *cp++; PUTHEX(c,fd);
				case 2: c = *cp++; PUTHEX(c,fd);
				case 1: c = *cp++; PUTHEX(c,fd);
				}
				cp += es;
			}
		}
	}
	_TIFFfree((char *) tf_buf);
}

void
PSDataColorSeparate(FILE* fd, TIFF* tif, uint32 w, uint32 h, int nc)
{
	uint32 row;
	int breaklen = MAXLINE, cc, s, maxs;
	unsigned char *tf_buf;
	unsigned char *cp, c;

	(void) w;
	tf_buf = (unsigned char *) _TIFFmalloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	maxs = (samplesperpixel > nc ? nc : samplesperpixel);
	for (row = 0; row < h; row++) {
		for (s = 0; s < maxs; s++) {
			if (TIFFReadScanline(tif, tf_buf, row, s) < 0)
				break;
			for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
				DOBREAK(breaklen, 1, fd);
				c = *cp++;
				PUTHEX(c,fd);
			}
		}
	}
	_TIFFfree((char *) tf_buf);
}

#define	PUTRGBHEX(c,fd) \
	PUTHEX(rmap[c],fd); PUTHEX(gmap[c],fd); PUTHEX(bmap[c],fd)

void
PSDataPalette(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	uint16 *rmap, *gmap, *bmap;
	uint32 row;
	int breaklen = MAXLINE, cc, nc;
	unsigned char *tf_buf;
	unsigned char *cp, c;

	(void) w;
	if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap)) {
		TIFFError(filename, "Palette image w/o \"Colormap\" tag");
		return;
	}
	switch (bitspersample) {
	case 8:	case 4: case 2: case 1:
		break;
	default:
		TIFFError(filename, "Depth %d not supported", bitspersample);
		return;
	}
	nc = 3 * (8 / bitspersample);
	tf_buf = (unsigned char *) _TIFFmalloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	if (checkcmap(tif, 1<<bitspersample, rmap, gmap, bmap) == 16) {
		int i;
#define	CVT(x)		(((x) * 255) / ((1U<<16)-1))
		for (i = (1<<bitspersample)-1; i >= 0; i--) {
			rmap[i] = CVT(rmap[i]);
			gmap[i] = CVT(gmap[i]);
			bmap[i] = CVT(bmap[i]);
		}
#undef CVT
	}
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
			DOBREAK(breaklen, nc, fd);
			switch (bitspersample) {
			case 8:
				c = *cp++; PUTRGBHEX(c, fd);
				break;
			case 4:
				c = *cp++; PUTRGBHEX(c&0xf, fd);
				c >>= 4;   PUTRGBHEX(c, fd);
				break;
			case 2:
				c = *cp++; PUTRGBHEX(c&0x3, fd);
				c >>= 2;   PUTRGBHEX(c&0x3, fd);
				c >>= 2;   PUTRGBHEX(c&0x3, fd);
				c >>= 2;   PUTRGBHEX(c, fd);
				break;
			case 1:
				c = *cp++; PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c, fd);
				break;
			}
		}
	}
	_TIFFfree((char *) tf_buf);
}

void
PSDataBW(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	int breaklen = MAXLINE;
	unsigned char* tf_buf;
	unsigned char* cp;
	tsize_t stripsize = TIFFStripSize(tif);
	tstrip_t s;

	(void) w; (void) h;
	tf_buf = (unsigned char *) _TIFFmalloc(stripsize);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	if (ascii85)
	        Ascii85Init();
	for (s = 0; s < TIFFNumberOfStrips(tif); s++) {
		int cc = TIFFReadEncodedStrip(tif, s, tf_buf, stripsize);
		if (cc < 0) {
			TIFFError(filename, "Can't read strip");
			break;
		}
		cp = tf_buf;
		if (photometric == PHOTOMETRIC_MINISWHITE) {
			for (cp += cc; --cp >= tf_buf;)
				*cp = ~*cp;
			cp++;
		}
		if (ascii85) {
			while (cc-- > 0)
				Ascii85Put(*cp++, fd);
		} else {
			while (cc-- > 0) {
				unsigned char c = *cp++;
				DOBREAK(breaklen, 1, fd);
				PUTHEX(c, fd);
			}
		}
	}
	if (ascii85)
	        Ascii85Flush(fd);
	else if (level2)
	        fputs(">\n", fd);
	_TIFFfree(tf_buf);
}

void
PSRawDataBW(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	uint32 *bc;
	uint32 bufsize;
	int breaklen = MAXLINE, cc;
	uint16 fillorder;
	unsigned char *tf_buf;
	unsigned char *cp, c;
	tstrip_t s;

	(void) w; (void) h;
	TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &fillorder);
	TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);
	bufsize = bc[0];
	tf_buf = (unsigned char*) _TIFFmalloc(bufsize);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for strip buffer");
		return;
	}
	for (s = 0; s < tf_numberstrips; s++) {
		if (bc[s] > bufsize) {
			tf_buf = (unsigned char *) _TIFFrealloc(tf_buf, bc[s]);
			if (tf_buf == NULL) {
				TIFFError(filename,
				    "No space for strip buffer");
				return;
			}
			bufsize = bc[s];
		}
		cc = TIFFReadRawStrip(tif, s, tf_buf, bc[s]);
		if (cc < 0) {
			TIFFError(filename, "Can't read strip");
			break;
		}
		if (fillorder == FILLORDER_LSB2MSB)
			TIFFReverseBits(tf_buf, cc);
		if (!ascii85) {
			for (cp = tf_buf; cc > 0; cc--) {
				DOBREAK(breaklen, 1, fd);
				c = *cp++;
				PUTHEX(c, fd);
			}
			fputs(">\n", fd);
			breaklen = MAXLINE;
		} else {
		        Ascii85Init();
			for (cp = tf_buf; cc > 0; cc--)
				Ascii85Put(*cp++, fd);
			Ascii85Flush(fd);
		}
	}
	_TIFFfree((char *) tf_buf);
}

void
Ascii85Init(void)
{
	ascii85breaklen = 2*MAXLINE;
	ascii85count = 0;
}

static char*
Ascii85Encode(unsigned char* raw)
{
	static char encoded[6];
	uint32 word;

	word = (((raw[0]<<8)+raw[1])<<16) + (raw[2]<<8) + raw[3];
	if (word != 0L) {
		uint32 q;
		uint16 w1;

		q = word / (85L*85*85*85);	/* actually only a byte */
		encoded[0] = q + '!';

		word -= q * (85L*85*85*85); q = word / (85L*85*85);
		encoded[1] = q + '!';

		word -= q * (85L*85*85); q = word / (85*85);
		encoded[2] = q + '!';

		w1 = (uint16) (word - q*(85L*85));
		encoded[3] = (w1 / 85) + '!';
		encoded[4] = (w1 % 85) + '!';
		encoded[5] = '\0';
	} else
		encoded[0] = 'z', encoded[1] = '\0';
	return (encoded);
}

void
Ascii85Put(unsigned char code, FILE* fd)
{
	ascii85buf[ascii85count++] = code;
	if (ascii85count >= 4) {
		unsigned char* p;
		int n;

		for (n = ascii85count, p = ascii85buf; n >= 4; n -= 4, p += 4) {
			char* cp;
			for (cp = Ascii85Encode(p); *cp; cp++) {
				putc(*cp, fd);
				if (--ascii85breaklen == 0) {
					putc('\n', fd);
					ascii85breaklen = 2*MAXLINE;
				}
			}
		}
		_TIFFmemcpy(ascii85buf, p, n);
		ascii85count = n;
	}
}

void
Ascii85Flush(FILE* fd)
{
	if (ascii85count > 0) {
		char* res;
		_TIFFmemset(&ascii85buf[ascii85count], 0, 3);
		res = Ascii85Encode(ascii85buf);
		fwrite(res[0] == 'z' ? "!!!!" : res, ascii85count + 1, 1, fd);
	}
	fputs("~>\n", fd);
}

char* stuff[] = {
"usage: tiff2ps [options] input.tif ...",
"where options are:",
" -1            generate PostScript Level I (default)",
" -2            generate PostScript Level II",
" -8            disable use of ASCII85 encoding with PostScript Level II",
" -d #          convert directory number #",
" -D            enable duplex printing (two pages per sheet of paper)",
" -e            generate Encapsulated PostScript (EPS)",
" -h #          assume printed page height is # inches (default 11)",
" -o #          convert directory at file offset #",
" -O file       write PostScript to file instead of standard output",
" -a            convert all directories in file (default is first)",
" -p            generate regular PostScript",
" -s            generate PostScript for a single image",
" -T            print pages for top edge binding",
" -w #          assume printed page width is # inches (default 8.5)",
" -z            enable printing in the deadzone (only for PostScript Level II)",
NULL
};

static void
usage(int code)
{
	char buf[BUFSIZ];
	int i;

	setbuf(stderr, buf);
	for (i = 0; stuff[i] != NULL; i++)
		fprintf(stderr, "%s\n", stuff[i]);
	exit(code);
}

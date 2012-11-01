/* MPGLIB replacement using mpega.library (AmigaOS)
 * Written by Thomas Wenzel and Sigbjørn (CISC) Skjæret.
 *
 * Big thanks to Stéphane Tavernard for mpega.library.
 *
 */

#ifdef AMIGA_MPEGA

#include "lame.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <dos.h>
#include <proto/mpega.h>

struct Library  *MPEGABase;
MPEGA_STREAM    *mstream=NULL;
MPEGA_CTRL      mctrl;


static int break_cleanup(void)
{
	/* Dummy break function to make atexit() work. :P */
	return 1;
}

static void exit_cleanup(void)
{
	if(mstream) {
		MPEGA_close(mstream);
		mstream = NULL;
	}
	if(MPEGABase) {
		CloseLibrary(MPEGABase);
		MPEGABase = NULL;
	}
}


int lame_decode_initfile(const char *fullname, int *stereo, int *samp, int *bitrate, unsigned long *nsamp)
{
	mctrl.bs_access = NULL;

	mctrl.layer_1_2.mono.quality    = 2;
	mctrl.layer_1_2.stereo.quality  = 2;
	mctrl.layer_1_2.mono.freq_div   = 1;
	mctrl.layer_1_2.stereo.freq_div = 1;
	mctrl.layer_1_2.mono.freq_max   = 48000;
	mctrl.layer_1_2.stereo.freq_max = 48000;
	mctrl.layer_3.mono.quality      = 2;
	mctrl.layer_3.stereo.quality    = 2;
	mctrl.layer_3.mono.freq_div     = 1;
	mctrl.layer_3.stereo.freq_div   = 1;
	mctrl.layer_3.mono.freq_max     = 48000;
	mctrl.layer_3.stereo.freq_max   = 48000;
	mctrl.layer_1_2.force_mono      = 0;
	mctrl.layer_3.force_mono        = 0;

	MPEGABase = OpenLibrary("mpega.library", 2);
	if(!MPEGABase) {
		fprintf(stderr, "Unable to open mpega.library v2\n");
		exit(1);
	}
	onbreak(break_cleanup);
	atexit(exit_cleanup);

	mstream=MPEGA_open(fullname, &mctrl);
	if(!mstream) { return (-1); }

	*stereo  = mstream->dec_channels;
	*samp    = mstream->dec_frequency;
	*bitrate = mstream->bitrate;
/*	*nsamp   = MAX_U_32_NUM; */
	*nsamp   = (FLOAT)mstream->ms_duration/1000 * mstream->dec_frequency;

	return 0;
}

int lame_decode_fromfile(FILE *fd, short pcm_l[],short pcm_r[])
{
	int outsize=0;
	WORD *b[MPEGA_MAX_CHANNELS];

	b[0]=pcm_l;
	b[1]=pcm_r;

	while (outsize == 0)
		outsize = MPEGA_decode_frame(mstream, b);

	if (outsize < 0) { return (-1); }
	else { return outsize; }
}

#endif /* AMIGA_MPEGA */

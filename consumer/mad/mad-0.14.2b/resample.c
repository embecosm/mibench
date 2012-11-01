/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: resample.c,v 1.5 2001/01/21 00:18:10 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <string.h>

# include "resample.h"
# include "mad.h"

static
mad_fixed_t const resample_table[9][9] = {
  /* 48000 */ { MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x116a3b36) /* 1.088435374 */,
		MAD_F(0x18000000) /* 1.500000000 */,
		MAD_F(0x20000000) /* 2.000000000 */,
		MAD_F(0x22d4766c) /* 2.176870748 */,
		MAD_F(0x30000000) /* 3.000000000 */,
		MAD_F(0x40000000) /* 4.000000000 */,
		MAD_F(0x45a8ecd8) /* 4.353741497 */,
		MAD_F(0x60000000) /* 6.000000000 */ },

  /* 44100 */ { MAD_F(0x0eb33333) /* 0.918750000 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x160ccccd) /* 1.378125000 */,
		MAD_F(0x1d666666) /* 1.837500000 */,
		MAD_F(0x20000000) /* 2.000000000 */,
		MAD_F(0x2c19999a) /* 2.756250000 */,
		MAD_F(0x3acccccd) /* 3.675000000 */,
		MAD_F(0x40000000) /* 4.000000000 */,
		MAD_F(0x58333333) /* 5.512500000 */ },

  /* 32000 */ { MAD_F(0x0aaaaaab) /* 0.666666667 */,
		MAD_F(0x0b9c2779) /* 0.725623583 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x15555555) /* 1.333333333 */,
		MAD_F(0x17384ef3) /* 1.451247166 */,
		MAD_F(0x20000000) /* 2.000000000 */,
		MAD_F(0x2aaaaaab) /* 2.666666667 */,
		MAD_F(0x2e709de5) /* 2.902494331 */,
		MAD_F(0x40000000) /* 4.000000000 */ },

  /* 24000 */ { MAD_F(0x08000000) /* 0.500000000 */,
		MAD_F(0x08b51d9b) /* 0.544217687 */,
		MAD_F(0x0c000000) /* 0.750000000 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x116a3b36) /* 1.088435374 */,
		MAD_F(0x18000000) /* 1.500000000 */,
		MAD_F(0x20000000) /* 2.000000000 */,
		MAD_F(0x22d4766c) /* 2.176870748 */,
		MAD_F(0x30000000) /* 3.000000000 */ },

  /* 22050 */ { MAD_F(0x0759999a) /* 0.459375000 */,
		MAD_F(0x08000000) /* 0.500000000 */,
		MAD_F(0x0b066666) /* 0.689062500 */,
		MAD_F(0x0eb33333) /* 0.918750000 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x160ccccd) /* 1.378125000 */,
		MAD_F(0x1d666666) /* 1.837500000 */,
		MAD_F(0x20000000) /* 2.000000000 */,
		MAD_F(0x2c19999a) /* 2.756250000 */ },

  /* 16000 */ { MAD_F(0x05555555) /* 0.333333333 */,
		MAD_F(0x05ce13bd) /* 0.362811791 */,
		MAD_F(0x08000000) /* 0.500000000 */,
		MAD_F(0x0aaaaaab) /* 0.666666667 */,
		MAD_F(0x0b9c2779) /* 0.725623583 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x15555555) /* 1.333333333 */,
		MAD_F(0x17384ef3) /* 1.451247166 */,
		MAD_F(0x20000000) /* 2.000000000 */ },

  /* 12000 */ { MAD_F(0x04000000) /* 0.250000000 */,
		MAD_F(0x045a8ecd) /* 0.272108844 */,
		MAD_F(0x06000000) /* 0.375000000 */,
		MAD_F(0x08000000) /* 0.500000000 */,
		MAD_F(0x08b51d9b) /* 0.544217687 */,
		MAD_F(0x0c000000) /* 0.750000000 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x116a3b36) /* 1.088435374 */,
		MAD_F(0x18000000) /* 1.500000000 */ },

  /* 11025 */ { MAD_F(0x03accccd) /* 0.229687500 */,
		MAD_F(0x04000000) /* 0.250000000 */,
		MAD_F(0x05833333) /* 0.344531250 */,
		MAD_F(0x0759999a) /* 0.459375000 */,
		MAD_F(0x08000000) /* 0.500000000 */,
		MAD_F(0x0b066666) /* 0.689062500 */,
		MAD_F(0x0eb33333) /* 0.918750000 */,
		MAD_F(0x10000000) /* 1.000000000 */,
		MAD_F(0x160ccccd) /* 1.378125000 */ },

  /*  8000 */ { MAD_F(0x02aaaaab) /* 0.166666667 */, 
		MAD_F(0x02e709de) /* 0.181405896 */, 
		MAD_F(0x04000000) /* 0.250000000 */, 
		MAD_F(0x05555555) /* 0.333333333 */, 
		MAD_F(0x05ce13bd) /* 0.362811791 */, 
		MAD_F(0x08000000) /* 0.500000000 */, 
		MAD_F(0x0aaaaaab) /* 0.666666667 */, 
		MAD_F(0x0b9c2779) /* 0.725623583 */, 
		MAD_F(0x10000000) /* 1.000000000 */ }
};

static
int rate_index(unsigned int rate)
{
  switch (rate) {
  case 48000: return 0;
  case 44100: return 1;
  case 32000: return 2;
  case 24000: return 3;
  case 22050: return 4;
  case 16000: return 5;
  case 12000: return 6;
  case 11025: return 7;
  case  8000: return 8;
  }

  return -1;
}

/*
 * NAME:	resample_init()
 * DESCRIPTION:	initialize resampling state
 */
int resample_init(struct resample_state *state,
		  unsigned int oldrate, unsigned int newrate)
{
  int oldi, newi;

  oldi = rate_index(oldrate);
  newi = rate_index(newrate);

  if (oldi == -1 || newi == -1)
    return -1;

  state->ratio = resample_table[oldi][newi];

  state->step = 0;
  state->last = 0;

  return 0;
}

/*
 * NAME:	resample_block()
 * DESCRIPTION:	algorithmically change the sampling rate of a PCM sample block
 */
unsigned int resample_block(struct resample_state *state,
			    unsigned int nsamples, mad_fixed_t const *old,
			    mad_fixed_t *new)
{
  mad_fixed_t const *end, *begin;

  /*
   * This resampling algorithm is based on a linear interpolation, which is
   * not at all the best sounding but is relatively fast and efficient.
   *
   * A better algorithm would be one that implements a bandlimited
   * interpolation.
   */

  if (state->ratio == MAD_F_ONE) {
    memcpy(new, old, nsamples * sizeof(mad_fixed_t));
    return nsamples;
  }

  end   = old + nsamples;
  begin = new;

  if (state->step < 0) {
    state->step = mad_f_fracpart(-state->step);

    while (state->step < MAD_F_ONE) {
      *new++ = state->step ?
	state->last + mad_f_mul(*old - state->last, state->step) : state->last;

      state->step += state->ratio;
      if (((state->step + 0x00000080L) & 0x0fffff00L) == 0)
	state->step = (state->step + 0x00000080L) & ~0x0fffffffL;
    }

    state->step -= MAD_F_ONE;
  }

  while (end - old > 1 + mad_f_intpart(state->step)) {
    old        += mad_f_intpart(state->step);
    state->step = mad_f_fracpart(state->step);

    *new++ = state->step ?
      *old + mad_f_mul(old[1] - old[0], state->step) : *old;

    state->step += state->ratio;
    if (((state->step + 0x00000080L) & 0x0fffff00L) == 0)
      state->step = (state->step + 0x00000080L) & ~0x0fffffffL;
  }

  if (end - old == 1 + mad_f_intpart(state->step)) {
    state->last = end[-1];
    state->step = -state->step;
  }
  else
    state->step -= mad_f_fromint(end - old);

  return new - begin;
}

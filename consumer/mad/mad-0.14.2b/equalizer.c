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
 * $Id: equalizer.c,v 1.3 2001/01/21 00:18:09 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include "equalizer.h"
# include "mad.h"

/*
 * NAME:	equalizer->init()
 * DESCRIPTION:	initialize an equalizer object
 */
void equalizer_init(struct equalizer *equalizer)
{
  int i;

  for (i = 0; i < 32; ++i) {
    equalizer->bandfactors[0][i] =
    equalizer->bandfactors[1][i] = MAD_F_ONE;
  }
}

/*
 * NAME:	equalizer_filter()
 * DESCRIPTION:	perform subband frequency equalization
 */
enum mad_flow equalizer_filter(void *data, struct mad_frame *frame)
{
  struct equalizer *equalizer = data;
  unsigned int nch, ch, ns, s, sb;

  nch = MAD_NCHANNELS(&frame->header);
  ns  = MAD_NSBSAMPLES(&frame->header);

  for (sb = 0; sb < 32; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      mad_fixed_t bandfactor;

      bandfactor = equalizer->bandfactors[ch][sb];

      if (bandfactor == MAD_F_ONE)
	continue;

      for (s = 0; s < ns; ++s) {
	frame->sbsample[ch][s][sb] =
	  mad_f_mul(frame->sbsample[ch][s][sb], bandfactor);
      }
    }
  }

  return MAD_FLOW_CONTINUE;
}

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
 * $Id: version.c,v 1.10 2001/10/18 05:12:25 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>

# include "version.h"
# include "mad.h"

static
void copyright(FILE *stream)
{
  fprintf(stream, "%s %s %s\n",
	  _("Copyright (C)"), MAD_PUBLISHYEAR, MAD_AUTHOR);
}

void ver_banner(FILE *stream)
{
  fprintf(stream, "%s %s - ", _("MPEG Audio Decoder"), MAD_VERSION);
  copyright(stream);

  fflush(stream);
}

void ver_license(FILE *stream)
{
  fputc('\n', stream);
  copyright(stream);

  fputc('\n', stream);
  fprintf(stream,
  _("This program is free software; you can redistribute it and/or modify it\n"
    "under the terms of the GNU General Public License as published by the\n"
    "Free Software Foundation; either version 2 of the License, or (at your\n"
    "option) any later version.\n\n"

    "This program is distributed in the hope that it will be useful, but\n"
    "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    "General Public License for more details.\n\n"

    "You should have received a copy of the GNU General Public License along\n"
    "with this program; if not, write to the Free Software Foundation, Inc.,\n"
    "59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n\n"

    "If you would like to negotiate alternate licensing terms, you may do so\n"
    "by contacting the author: %s <%s>\n"), MAD_AUTHOR, MAD_EMAIL);

  fputc('\n', stream);
}

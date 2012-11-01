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
 * $Id: acconfig.h,v 1.13 2001/01/21 00:18:09 rob Exp $
 */

# ifndef CONFIG_H
# define CONFIG_H

/*****************************************************************************
 * Definitions selected automatically by `configure'                         *
 *****************************************************************************/
@TOP@

/* Define to the name of the distribution.  */
#undef PACKAGE

/* Define to the version of the distribution.  */
#undef VERSION

/* Define to be the name of your default audio output module. */
#define AUDIO_DEFAULT 0

/* Define if you have mmap() and you want to use it. */
#undef USE_MMAP

/* Define to 1 if NLS is requested.  */
#undef ENABLE_NLS

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
#undef HAVE_CATGETS

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
#undef HAVE_GETTEXT

/* Define if your locale.h file contains LC_MESSAGES.  */
#undef HAVE_LC_MESSAGES

/* Define as 1 if you have the stpcpy function.  */
#undef HAVE_STPCPY

/* Define to enable diagnostic debugging support. */
#undef DEBUG

/* Define to disable debugging assertions. */
#undef NDEBUG

/* Define to enable experimental code. */
#undef EXPERIMENTAL

@BOTTOM@
/*****************************************************************************
 * End of automatically configured definitions                               *
 *****************************************************************************/

# endif

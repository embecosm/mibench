/*
 * libid3tag - ID3 tag manipulation library
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
 * $Id: config.h,v 1.6 2001/11/09 02:35:31 rob Exp $
 */

# ifndef LIBID3TAG_CONFIG_H
# define LIBID3TAG_CONFIG_H

/*****************************************************************************
 * Definitions selected automatically by `configure'                         *
 *****************************************************************************/

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define as __inline if that's what the C compiler calls it.  */
#define inline __inline

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define to enable diagnostic debugging support. */
/* #undef DEBUG */

/* Define to disable debugging assertions. */
/* #undef NDEBUG */

/* Define if you have the <assert.h> header file.  */
#define HAVE_ASSERT_H 1

/* Define if you have the <unistd.h> header file.  */
/* #undef HAVE_UNISTD_H */

/* Define if you have the <zlib.h> header file.  */
/* #undef HAVE_ZLIB_H */

/* Name of package */
#define PACKAGE "libid3tag"

/* Version number of package */
#define VERSION "0.14.2b"

/*****************************************************************************
 * End of automatically configured definitions                               *
 *****************************************************************************/

# endif

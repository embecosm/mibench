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
 * $Id: genre.c,v 1.1 2001/10/22 19:52:59 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include "id3tag.h"
# include "ucs4.h"

/* genres are stored in ucs4 format */
# include "genre.dat"

# define NGENRES (sizeof(genre_table) / sizeof(genre_table[0]))

/*
 * NAME:	genre->name()
 * DESCRIPTION:	translate a genre number/keyword to its full name
 */
id3_ucs4_t const *id3_genre_name(id3_ucs4_t const *string)
{
  id3_ucs4_t const *ptr;
  static id3_ucs4_t const genre_remix[] = { 'R', 'e', 'm', 'i', 'x', 0 };
  static id3_ucs4_t const genre_cover[] = { 'C', 'o', 'v', 'e', 'r', 0 };
  unsigned long number;

  if (string == 0 || *string == 0)
    return id3_ucs4_empty;

  if (string[0] == 'R' && string[1] == 'X' && string[2] == 0)
    return genre_remix;
  if (string[0] == 'C' && string[1] == 'R' && string[2] == 0)
    return genre_cover;

  for (ptr = string; *ptr; ++ptr) {
    if (*ptr < '0' || *ptr > '9')
      return string;
  }

  number = id3_ucs4_getnumber(string);

  return (number < NGENRES) ? genre_table[number] : id3_ucs4_empty;
}

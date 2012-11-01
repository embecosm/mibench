/* C code produced by gperf version 2.7.1 (19981006 egcs) */
/* Command-line: gperf -tCcTonD -K id -N id3_frametype_lookup -s -3 -k * ../../../libid3tag/frametype.gperf  */
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
 * Id: frametype.gperf,v 1.3 2001/10/20 22:15:47 rob Exp 
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <string.h>

# include "id3tag.h"
# include "frametype.h"

# define FIELDS(id)  static enum id3_field_type const fields_##id[]

/* frame field descriptions */

FIELDS(UFID) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(TXXX) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_STRING
};

FIELDS(WXXX) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_LATIN1
};

FIELDS(MCDI) = {
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(ETCO) = {
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(MLLT) = {
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_INT24,
  ID3_FIELD_TYPE_INT24,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(SYTC) = {
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(USLT) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LANGUAGE,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_STRINGFULL
};

FIELDS(SYLT) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LANGUAGE,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(COMM) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LANGUAGE,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_STRINGFULL
};

FIELDS(RVA2) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(EQU2) = {
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(RVRB) = {
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT8
};

FIELDS(APIC) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(GEOB) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(PCNT) = {
  ID3_FIELD_TYPE_INT32PLUS
};

FIELDS(POPM) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT32PLUS
};

FIELDS(RBUF) = {
  ID3_FIELD_TYPE_INT24,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT32
};

FIELDS(AENC) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(LINK) = {
  ID3_FIELD_TYPE_FRAMEID,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_LATIN1LIST
};

FIELDS(POSS) = {
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(USER) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LANGUAGE,
  ID3_FIELD_TYPE_STRING
};

FIELDS(OWNE) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_DATE,
  ID3_FIELD_TYPE_STRING
};

FIELDS(COMR) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_DATE,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(ENCR) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(GRID) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(PRIV) = {
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(SIGN) = {
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(SEEK) = {
  ID3_FIELD_TYPE_INT32
};

FIELDS(ASPI) = {
  ID3_FIELD_TYPE_INT32,
  ID3_FIELD_TYPE_INT32,
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(text) = {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_STRINGLIST
};

FIELDS(url) = {
  ID3_FIELD_TYPE_LATIN1
};

FIELDS(unknown) = {
  ID3_FIELD_TYPE_BINARYDATA
};

FIELDS(ZOBS) = {
  ID3_FIELD_TYPE_FRAMEID,
  ID3_FIELD_TYPE_BINARYDATA
};

# define FRAME(id)  \
  sizeof(fields_##id) / sizeof(fields_##id[0]), fields_##id

# define PRESERVE  0
# define DISCARD   ID3_FRAME_FLAG_FILEALTERPRESERVATION
# define OBSOLETE  (DISCARD | ID3_FRAME_FLAG_TAGALTERPRESERVATION)

# define FRAMETYPE(type, id, flags, desc)  \
  struct id3_frametype const id3_frametype_##type = {  \
    0, FRAME(id), flags, desc  \
  }

/* static frame types */

FRAMETYPE(text,         text,    PRESERVE, "Unknown text information frame");
FRAMETYPE(url,          url,     PRESERVE, "Unknown URL link frame");
FRAMETYPE(experimental, unknown, PRESERVE, "Experimental frame");
FRAMETYPE(unknown,      unknown, PRESERVE, "Unknown frame");
FRAMETYPE(obsolete,     unknown, OBSOLETE, "Obsolete frame");

#define TOTAL_KEYWORDS 84
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 0
#define MAX_HASH_VALUE 111
/* maximum key range = 112, duplicates = 12 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112,  20,
       18,   2,  25, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112,  19,  26,   1,  28,  15,
        9,  31, 112,   0, 112,  29,   3,   0,  10,   0,
       30,   0,   8,   5,   0,  29,  31,  24,  29,  24,
       30, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112, 112, 112, 112, 112,
      112, 112, 112, 112, 112, 112
    };
  register int hval = 0;

  switch (len)
    {
      default:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const struct id3_frametype *
id3_frametype_lookup (str, len)
     register const char *str;
     register unsigned int len;
{
  static const struct id3_frametype wordlist[] =
    {
      {"TMOO", FRAME(text), PRESERVE, "Mood"},
      {"TCOM", FRAME(text), PRESERVE, "Composer"},
      {"COMM", FRAME(COMM), PRESERVE, "Comments"},
      {"TIT3", FRAME(text), PRESERVE, "Subtitle/description refinement"},
      {"TMCL", FRAME(text), PRESERVE, "Musician credits list"},
      {"TSOT", FRAME(text), PRESERVE, "Title sort order"},
      {"MLLT", FRAME(MLLT), DISCARD,  "MPEG location lookup table"},
      {"COMR", FRAME(COMR), PRESERVE, "Commercial frame"},
      {"TSST", FRAME(text), PRESERVE, "Set subtitle"},
      {"TCON", FRAME(text), PRESERVE, "Content type"},
      {"TFLT", FRAME(text), PRESERVE, "File type"},
      {"TRSO", FRAME(text), PRESERVE, "Internet radio station owner"},
      {"TSRC", FRAME(text), PRESERVE, "ISRC (international standard recording code)"},
      {"ETCO", FRAME(ETCO), DISCARD,  "Event timing codes"},
      {"TIT2", FRAME(text), PRESERVE, "Title/songname/content description"},
      {"TOFN", FRAME(text), PRESERVE, "Original filename"},
      {"TIT1", FRAME(text), PRESERVE, "Content group description"},
      {"TOAL", FRAME(text), PRESERVE, "Original album/movie/show title"},
      {"TRSN", FRAME(text), PRESERVE, "Internet radio station name"},
      {"TSOA", FRAME(text), PRESERVE, "Album sort order"},
      {"TSSE", FRAME(text), PRESERVE, "Software/hardware and settings used for encoding"},
      {"WCOM", FRAME(url),  PRESERVE, "Commercial information"},
      {"TENC", FRAME(text), DISCARD,  "Encoded by"},
      {"TOLY", FRAME(text), PRESERVE, "Original lyricist(s)/text writer(s)"},
      {"TLEN", FRAME(text), DISCARD,  "Length"},
      {"MCDI", FRAME(MCDI), PRESERVE, "Music CD identifier"},
      {"SYTC", FRAME(SYTC), DISCARD,  "Synchronised tempo codes"},
      {"TCOP", FRAME(text), PRESERVE, "Copyright message"},
      {"SYLT", FRAME(SYLT), DISCARD,  "Synchronised lyric/text"},
      {"TLAN", FRAME(text), PRESERVE, "Language(s)"},
      {"TIPL", FRAME(text), PRESERVE, "Involved people list"},
      {"ENCR", FRAME(ENCR), PRESERVE, "Encryption method registration"},
      {"TOWN", FRAME(text), PRESERVE, "File owner/licensee"},
      {"TPOS", FRAME(text), PRESERVE, "Part of a set"},
      {"TSOP", FRAME(text), PRESERVE, "Performer sort order"},
      {"TDOR", FRAME(text), PRESERVE, "Original release time"},
      {"TDRC", FRAME(text), PRESERVE, "Recording time"},
      {"WORS", FRAME(url),  PRESERVE, "Official Internet radio station homepage"},
      {"USLT", FRAME(USLT), PRESERVE, "Unsynchronised lyric/text transcription"},
      {"TRCK", FRAME(text), PRESERVE, "Track number/position in set"},
      {"TPRO", FRAME(text), PRESERVE, "Produced notice"},
      {"TDRL", FRAME(text), PRESERVE, "Release time"},
      {"POSS", FRAME(POSS), DISCARD,  "Position synchronisation frame"},
      {"PCNT", FRAME(PCNT), PRESERVE, "Play counter"},
      {"LINK", FRAME(LINK), PRESERVE, "Linked information"},
      {"TMED", FRAME(text), PRESERVE, "Media type"},
      {"TEXT", FRAME(text), PRESERVE, "Lyricist/text writer"},
      {"TOPE", FRAME(text), PRESERVE, "Original artist(s)/performer(s)"},
      {"AENC", FRAME(AENC), DISCARD,  "Audio encryption"},
      {"SIGN", FRAME(SIGN), PRESERVE, "Signature frame"},
      {"TPE3", FRAME(text), PRESERVE, "Conductor/performer refinement"},
      {"WOAS", FRAME(url),  PRESERVE, "Official audio source webpage"},
      {"TALB", FRAME(text), PRESERVE, "Album/movie/show title"},
      {"OWNE", FRAME(OWNE), PRESERVE, "Ownership frame"},
      {"APIC", FRAME(APIC), PRESERVE, "Attached picture"},
      {"WOAR", FRAME(url),  PRESERVE, "Official artist/performer webpage"},
      {"WOAF", FRAME(url),  PRESERVE, "Official audio file webpage"},
      {"TDEN", FRAME(text), PRESERVE, "Encoding time"},
      {"ASPI", FRAME(ASPI), DISCARD,  "Audio seek point index"},
      {"WCOP", FRAME(url),  PRESERVE, "Copyright/legal information"},
      {"TDLY", FRAME(text), PRESERVE, "Playlist delay"},
      {"TBPM", FRAME(text), PRESERVE, "BPM (beats per minute)"},
      {"USER", FRAME(USER), PRESERVE, "Terms of use"},
      {"TDTG", FRAME(text), PRESERVE, "Tagging time"},
      {"POPM", FRAME(POPM), PRESERVE, "Popularimeter"},
      {"ZOBS", FRAME(ZOBS), OBSOLETE, "Obsolete frame"},
      {"EQU2", FRAME(EQU2), DISCARD,  "Equalisation (2)"},
      {"TPE2", FRAME(text), PRESERVE, "Band/orchestra/accompaniment"},
      {"SEEK", FRAME(SEEK), DISCARD,  "Seek frame"},
      {"TPE1", FRAME(text), PRESERVE, "Lead performer(s)/soloist(s)"},
      {"UFID", FRAME(UFID), PRESERVE, "Unique file identifier"},
      {"GRID", FRAME(GRID), PRESERVE, "Group identification registration"},
      {"TKEY", FRAME(text), PRESERVE, "Initial key"},
      {"PRIV", FRAME(PRIV), PRESERVE, "Private frame"},
      {"TPE4", FRAME(text), PRESERVE, "Interpreted, remixed, or otherwise modified by"},
      {"GEOB", FRAME(GEOB), PRESERVE, "General encapsulated object"},
      {"RBUF", FRAME(RBUF), PRESERVE, "Recommended buffer size"},
      {"RVRB", FRAME(RVRB), PRESERVE, "Reverb"},
      {"RVA2", FRAME(RVA2), DISCARD,  "Relative volume adjustment (2)"},
      {"TPUB", FRAME(text), PRESERVE, "Publisher"},
      {"TXXX", FRAME(TXXX), PRESERVE, "User defined text information frame"},
      {"WPAY", FRAME(url),  PRESERVE, "Payment"},
      {"WPUB", FRAME(url),  PRESERVE, "Publishers official webpage"},
      {"WXXX", FRAME(WXXX), PRESERVE, "User defined URL link frame"}
    };

  static const short lookup[] =
    {
         0,  -92,    3,   -1,    4,    5,    6,  -83,
        -2,    7,    8,    9,   10,   11,   12,   -1,
        13,   -1,   14,   15,   16,   -1,   17,   18,
        19, -183,   22,   23,   24,   25,   26,   27,
      -179,   30, -177, -175,   35, -173, -168,   41,
        42,   43,   44,   45,   46, -166,   49,   50,
      -164,   53,   54,   55,   56,   57,   58, -162,
        61,   62,   -1,   63,   64,   65,   66,   67,
        68,   69,   70,   71,   72,   73,   74,   -1,
      -159,   77,   -9,   -2,   78,  -25,   -2,  -33,
        -2,  -37,   -2,  -45,   -2,   79,   -1,   80,
       -48,   -3,  -51,   -2,  -53,   -2,  -56,   -2,
        -1,   81,  -64,   -2,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   82,   -1,   83
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].id;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                return &wordlist[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct id3_frametype *wordptr = &wordlist[TOTAL_KEYWORDS + lookup[offset]];
              register const struct id3_frametype *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->id;

                  if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}

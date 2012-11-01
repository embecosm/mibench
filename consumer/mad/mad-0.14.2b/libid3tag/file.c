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
 * $Id: file.c,v 1.8 2001/11/01 20:27:38 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

# include "id3tag.h"
# include "file.h"
# include "tag.h"
# include "field.h"

struct filetag {
  struct id3_tag *tag;
  unsigned long location;
  id3_length_t length;
};

struct id3_file {
  FILE *iofile;
  enum id3_file_mode mode;
  int flags;
  int options;
  struct id3_tag *primary;
  unsigned int ntags;
  struct filetag *tags;
};

enum {
  ID3_FILE_FLAG_ID3V1 = 0x0001
};

/*
 * NAME:	query_tag()
 * DESCRIPTION:	check for a tag at a file's current position
 */
static
signed long query_tag(FILE *iofile)
{
  fpos_t save_position;
  id3_byte_t query[ID3_TAG_QUERYSIZE];
  signed long size;

  if (fgetpos(iofile, &save_position) == -1)
    return 0;

  size = id3_tag_query(query, fread(query, 1, sizeof(query), iofile));

  if (fsetpos(iofile, &save_position) == -1)
    return 0;

  return size;
}

/*
 * NAME:	read_tag()
 * DESCRIPTION:	read and parse a tag at a file's current position
 */
static
struct id3_tag *read_tag(FILE *iofile, id3_length_t size)
{
  id3_byte_t *data;
  struct id3_tag *tag = 0;

  data = malloc(size);
  if (data) {
    if (fread(data, size, 1, iofile) == 1)
      tag = id3_tag_parse(data, size);

    free(data);
  }

  return tag;
}

/*
 * NAME:	update_primary()
 * DESCRIPTION:	update the primary tag with data from a new tag
 */
static
int update_primary(struct id3_tag *tag, struct id3_tag const *new)
{
  unsigned int i;

  if (!(new->extendedflags & ID3_TAG_EXTENDEDFLAG_TAGISANUPDATE))
    id3_tag_clearframes(tag);

  for (i = 0; i < new->nframes; ++i) {
    if (id3_tag_attachframe(tag, new->frames[i]) == -1)
      return -1;
  }

  return 0;
}

/*
 * NAME:	add_tag()
 * DESCRIPTION:	read, parse, and add a tag to a file structure
 */
static
int add_tag(struct id3_file *file, id3_length_t length)
{
  long location;
  unsigned int i;
  struct filetag filetag, *tags;
  struct id3_tag *tag;

  location = ftell(file->iofile);
  if (location == -1)
    return -1;

  /* check for duplication/overlap */
  {
    unsigned long begin1, end1, begin2, end2;

    begin1 = location;
    end1   = begin1 + length;

    for (i = 0; i < file->ntags; ++i) {
      begin2 = file->tags[i].location;
      end2   = begin2 + file->tags[i].length;

      if (begin1 == begin2 && end1 == end2)
	return 0;  /* duplicate */

      if (begin1 < end2 && end1 > begin2)
	return -1;  /* overlap */
    }
  }

  tags = realloc(file->tags, (file->ntags + 1) * sizeof(*tags));
  if (tags == 0)
    return -1;

  file->tags = tags;

  tag = read_tag(file->iofile, length);
  if (tag == 0)
    return -1;

  if (update_primary(file->primary, tag) == -1) {
    id3_tag_delete(tag);
    return -1;
  }

  filetag.tag      = tag;
  filetag.location = location;
  filetag.length   = length;

  file->tags[file->ntags++] = filetag;

  id3_tag_addref(tag);

  return 0;
}

/*
 * NAME:	search_tags()
 * DESCRIPTION:	search for tags in a file
 */
static
int search_tags(struct id3_file *file)
{
  fpos_t save_position;
  signed long size;
  int result = 0;

  if (fgetpos(file->iofile, &save_position) == -1)
    return -1;

  /* look for an ID3v1 tag */

  if (fseek(file->iofile, -128, SEEK_END) == 0) {
    size = query_tag(file->iofile);
    if (size > 0) {
      if (add_tag(file, size) == -1)
	goto fail;

      file->flags   |= ID3_FILE_FLAG_ID3V1;
      file->options |= ID3_FILE_OPTION_ID3V1;
    }
  }

  /* look for a tag at the beginning of the file */

  rewind(file->iofile);

  size = query_tag(file->iofile);
  if (size > 0) {
    struct id3_frame const *frame;

    if (add_tag(file, size) == -1)
      goto fail;

    /* locate tags indicated by SEEK frames */

    while ((frame =
	    id3_tag_findframe(file->tags[file->ntags - 1].tag, "SEEK", 0))) {
      long seek;

      seek = id3_field_getint(&frame->fields[0]);
      if (seek < 0 || fseek(file->iofile, seek, SEEK_CUR) == -1)
	break;

      size = query_tag(file->iofile);
      if (size <= 0)
	break;
      else if (add_tag(file, size) == -1)
	goto fail;
    }
  }

  /* look for a tag at the end of the file (before any ID3v1 tag) */

  if (fseek(file->iofile, ((file->flags & ID3_FILE_FLAG_ID3V1) ? -128 : 0) +
	    -10, SEEK_END) == 0) {
    size = query_tag(file->iofile);
    if (size < 0 && fseek(file->iofile, size, SEEK_CUR) == 0) {
      size = query_tag(file->iofile);
      if (size > 0 && add_tag(file, size) == -1)
	goto fail;
    }
  }

  if (0) {
  fail:
    result = -1;
  }

  clearerr(file->iofile);

  if (fsetpos(file->iofile, &save_position) == -1)
    return -1;

  return result;
}

/*
 * NAME:	finish_file()
 * DESCRIPTION:	release memory associated with a file
 */
static
void finish_file(struct id3_file *file)
{
  unsigned int i;

  if (file->primary) {
    id3_tag_delref(file->primary);
    id3_tag_delete(file->primary);
  }

  for (i = 0; i < file->ntags; ++i) {
    id3_tag_delref(file->tags[i].tag);
    id3_tag_delete(file->tags[i].tag);
  }

  if (file->tags)
    free(file->tags);

  free(file);
}

/*
 * NAME:	new_file()
 * DESCRIPTION:	create a new file structure and load tags
 */
static
struct id3_file *new_file(FILE *iofile, enum id3_file_mode mode)
{
  struct id3_file *file;

  file = malloc(sizeof(*file));
  if (file == 0)
    goto fail;

  file->iofile  = iofile;
  file->mode    = mode;
  file->flags   = 0;
  file->options = 0;

  file->ntags   = 0;
  file->tags    = 0;

  file->primary = id3_tag_new();
  if (file->primary == 0)
    goto fail;

  id3_tag_addref(file->primary);

  /* load tags from the file */

  if (search_tags(file) == -1)
    goto fail;

  if (0) {
  fail:
    if (file) {
      finish_file(file);
      file = 0;
    }
  }

  return file;
}

/*
 * NAME:	file->open()
 * DESCRIPTION:	open a file given its pathname
 */
struct id3_file *id3_file_open(char const *path, enum id3_file_mode mode)
{
  FILE *iofile;
  struct id3_file *file;

  iofile = fopen(path, (mode == ID3_FILE_MODE_READWRITE) ? "r+b" : "rb");
  if (iofile == 0)
    return 0;

  file = new_file(iofile, mode);
  if (file == 0)
    fclose(iofile);

  return file;
}

/*
 * NAME:	file->fdopen()
 * DESCRIPTION:	open a file using an existing file descriptor
 */
struct id3_file *id3_file_fdopen(int fd, enum id3_file_mode mode)
{
# if 1 || defined(HAVE_UNISTD_H)
  FILE *iofile;
  struct id3_file *file;

  iofile = fdopen(fd, (mode == ID3_FILE_MODE_READWRITE) ? "r+b" : "rb");
  if (iofile == 0)
    return 0;

  file = new_file(iofile, mode);
  if (file == 0) {
    int save_fd;

    /* close iofile without closing fd */

    save_fd = dup(fd);

    fclose(iofile);

    dup2(save_fd, fd);
    close(save_fd);
  }

  return file;
# else
  return 0;
# endif
}

/*
 * NAME:	file->close()
 * DESCRIPTION:	close a file and delete its associated tags
 */
void id3_file_close(struct id3_file *file)
{
  fclose(file->iofile);

  finish_file(file);
}

/*
 * NAME:	file->tag()
 * DESCRIPTION:	return the primary tag structure for a file
 */
struct id3_tag *id3_file_tag(struct id3_file const *file)
{
  return file->primary;
}

/*
 * NAME:	file->update()
 * DESCRIPTION:	rewrite tag(s) to a file
 */
int id3_file_update(struct id3_file *file)
{
  id3_length_t size;
  unsigned char id3v1_data[128], *id3v1 = 0, *id3v2 = 0;

  if (file->mode != ID3_FILE_MODE_READWRITE)
    return -1;

  if (file->options & ID3_FILE_OPTION_ID3V1) {
    file->primary->options |= ID3_TAG_OPTION_ID3V1;

    size = id3_tag_render(file->primary, 0);
    if (size) {
      assert(size == sizeof(id3v1_data));

      size = id3_tag_render(file->primary, id3v1_data);
      if (size) {
	assert(size == sizeof(id3v1_data));
	id3v1 = id3v1_data;
      }
    }
  }

  file->primary->options &= ~ID3_TAG_OPTION_ID3V1;

  size = id3_tag_render(file->primary, 0);
  if (size) {
    id3v2 = malloc(size);
    if (id3v2 == 0)
      return -1;

    size = id3_tag_render(file->primary, id3v2);
    if (size == 0) {
      free(id3v2);
      id3v2 = 0;
    }
  }

  /* ... */

  if (id3v2)
    free(id3v2);

  return 0;
}

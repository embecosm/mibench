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
 * $Id: player.c,v 1.54 2001/11/09 04:54:34 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <stdarg.h>
# include <stdlib.h>

# ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

# include <sys/stat.h>

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>
# endif

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

# include <string.h>

# ifdef HAVE_ERRNO_H
#  include <errno.h>
# endif

# include <time.h>
# include <locale.h>
# include <math.h>

# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif

# ifdef _WIN32
#  include <windows.h>
# endif

# include <signal.h>

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# if defined(HAVE_MMAP)
#  include <sys/mman.h>
# endif

# if !defined(O_BINARY)
#  define O_BINARY  0
# endif

# include "player.h"
# include "mad.h"
# include "audio.h"
# include "resample.h"
# include "id3tag.h"
# include "filter.h"
# include "equalizer.h"

# define MPEG_BUFSZ	40000	/* 2.5 s at 128 kbps; 1 s at 320 kbps */
# define FREQ_TOLERANCE	6	/* percent sampling frequency tolerance */

# define TTY_DEVICE	"/dev/tty"

# define KEY_CTRL(key)	((key) & 0x1f)

enum {
  KEY_PAUSE    = 'p',
  KEY_STOP     = 's',
  KEY_FORWARD  = 'f',
  KEY_BACK     = 'b',
  KEY_TIME     = 't',
  KEY_QUIT     = 'q',
  KEY_INFO     = 'i'
};

static int on_same_line;

# if defined(USE_TTY) && !defined(_WIN32)
static int tty_fd = -1;
static struct termios save_tty;
static struct sigaction save_sigtstp, save_sigint;
# endif

/*
 * NAME:	player_init()
 * DESCRIPTION:	initialize player structure
 */
void player_init(struct player *player)
{
  player->verbosity = 0;

  player->options  = 0;
  player->repeat   = 1;

  player->control = PLAYER_CONTROL_DEFAULT;

  player->playlist.entries = 0;
  player->playlist.length  = 0;
  player->playlist.current = 0;

  player->global_start = mad_timer_zero;
  player->global_stop  = mad_timer_zero;

  player->fade_in      = mad_timer_zero;
  player->fade_out     = mad_timer_zero;
  player->gap          = mad_timer_zero;

  player->input.path        = 0;
  player->input.fd          = -1;
# if defined(HAVE_MMAP)
  player->input.fdm         = 0;
# endif
  player->input.data        = 0;
  player->input.length      = 0;
  player->input.eof         = 0;

  xing_init(&player->input.xing);

  player->output.mode          = AUDIO_MODE_DITHER;
  player->output.attenuation   = MAD_F_ONE;
  player->output.filters       = 0;
  player->output.channels_in   = 0;
  player->output.channels_out  = 0;
  player->output.select        = PLAYER_CHANNEL_DEFAULT;
  player->output.speed_in      = 0;
  player->output.speed_out     = 0;
  player->output.precision_in  = 0;
  player->output.precision_out = 0;
  player->output.path          = 0;
  player->output.command       = 0;
  /* player->output.resample */
  player->output.resampled     = 0;

  player->stats.show                  = STATS_SHOW_OVERALL;
  player->stats.label                 = 0;
  player->stats.total_bytes           = 0;
  player->stats.total_time            = mad_timer_zero;
  player->stats.global_timer          = mad_timer_zero;
  player->stats.absolute_timer        = mad_timer_zero;
  player->stats.play_timer            = mad_timer_zero;
  player->stats.global_framecount     = 0;
  player->stats.absolute_framecount   = 0;
  player->stats.play_framecount       = 0;
  player->stats.error_frame           = -1;
  player->stats.mute_frame            = 0;
  player->stats.vbr                   = 0;
  player->stats.bitrate               = 0;
  player->stats.vbr_frames            = 0;
  player->stats.vbr_rate              = 0;
  player->stats.nsecs                 = 0;
  player->stats.audio.clipped_samples = 0;
  player->stats.audio.peak_clipping   = 0;
  player->stats.audio.peak_sample     = 0;
}

/*
 * NAME:	player_finish()
 * DESCRIPTION:	destroy a player structure
 */
void player_finish(struct player *player)
{
  if (player->output.filters)
    filter_free(player->output.filters);

  if (player->output.resampled) {
    resample_finish(&player->output.resample[0]);
    resample_finish(&player->output.resample[1]);

    free(player->output.resampled);
    player->output.resampled = 0;
  }
}

/*
 * NAME:	message()
 * DESCRIPTION:	show a message, possibly overwriting a previous w/o newline
 */
static
int message(char const *format, ...)
{
  int len, newline, result;
  va_list args;

  len = strlen(format);
  newline = (len > 0 && format[len - 1] == '\n');

  if (on_same_line && newline && len > 1)
    fputc('\n', stderr);

  va_start(args, format);
  result = vfprintf(stderr, format, args);
  va_end(args);

  if (on_same_line && !newline && result < on_same_line) {
    unsigned int i;

    i = on_same_line - result;
    while (i--)
      putc(' ', stderr);
  }

  on_same_line = newline ? 0 : result;

  if (!newline) {
    fputc('\r', stderr);
    fflush(stderr);
  }

  return result;
}

/*
 * NAME:	error()
 * DESCRIPTION:	show an error using proper interaction with message()
 */
static
void error(char const *id, char const *format, ...)
{
  int err;
  va_list args;

  err = errno;

  if (on_same_line)
    message("\n");

  if (id)
    fprintf(stderr, "%s: ", id);

  va_start(args, format);

  if (*format == ':') {
    if (format[1] == 0) {
      format = va_arg(args, char const *);
      errno = err;
      perror(format);
    }
    else {
      errno = err;
      perror(format + 1);
    }
  }
  else {
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
  }

  va_end(args);
}

# if defined(HAVE_MMAP)
/*
 * NAME:	map_file()
 * DESCRIPTION:	map the contents of a file into memory
 */
static
void *map_file(int fd, unsigned long length)
{
  void *fdm;

  fdm = mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
  if (fdm == MAP_FAILED)
    return 0;

# if defined(HAVE_MADVISE)
  madvise(fdm, length, MADV_SEQUENTIAL);
# endif

  return fdm;
}

/*
 * NAME:	unmap_file()
 * DESCRIPTION:	undo a file mapping
 */
static
int unmap_file(void *fdm, unsigned long length)
{
  if (munmap(fdm, length) == -1)
    return -1;

  return 0;
}

/*
 * NAME:	decode->input_mmap()
 * DESCRIPTION:	(re)fill decoder input buffer from a memory map
 */
static
enum mad_flow decode_input_mmap(void *data, struct mad_stream *stream)
{
  struct player *player = data;
  struct input *input = &player->input;

  if (input->eof)
    return MAD_FLOW_STOP;

  if (stream->next_frame) {
    struct stat stat;
    unsigned long posn, left;

    if (fstat(input->fd, &stat) == -1)
      return MAD_FLOW_BREAK;

    posn = stream->next_frame - input->fdm;

    /* check for file size change and update map */

    if (stat.st_size > input->length) {
      if (unmap_file(input->fdm, input->length) == -1) {
	input->fdm  = 0;
	input->data = 0;
	return MAD_FLOW_BREAK;
      }

      player->stats.total_bytes += stat.st_size - input->length;

      input->length = stat.st_size;

      input->fdm = map_file(input->fd, input->length);
      if (input->fdm == 0) {
	input->data = 0;
	return MAD_FLOW_BREAK;
      }

      mad_stream_buffer(stream, input->fdm + posn, input->length - posn);

      return MAD_FLOW_CONTINUE;
    }

    /* end of memory map; append MAD_BUFFER_GUARD zero bytes */

    left = input->length - posn;

    input->data = malloc(left + MAD_BUFFER_GUARD);
    if (input->data == 0)
      return MAD_FLOW_BREAK;

    input->eof = 1;

    memcpy(input->data, input->fdm + posn, left);
    memset(input->data + left, 0, MAD_BUFFER_GUARD);

    mad_stream_buffer(stream, input->data, left + MAD_BUFFER_GUARD);

    return MAD_FLOW_CONTINUE;
  }

  /* first call */

  mad_stream_buffer(stream, input->fdm, input->length);

  return MAD_FLOW_CONTINUE;
}
# endif

/*
 * NAME:	decode->input_read()
 * DESCRIPTION:	(re)fill decoder input buffer by reading a file descriptor
 */
static
enum mad_flow decode_input_read(void *data, struct mad_stream *stream)
{
  struct player *player = data;
  struct input *input = &player->input;
  int len;

  if (input->eof)
    return MAD_FLOW_STOP;

  if (stream->next_frame) {
    memmove(input->data, stream->next_frame,
	    input->length = &input->data[input->length] - stream->next_frame);
  }

  do {
    len = read(input->fd, input->data + input->length,
	       MPEG_BUFSZ - input->length);
  }
  while (len == -1 && errno == EINTR);

  if (len == -1) {
    error("input", ":read");
    return MAD_FLOW_BREAK;
  }
  else if (len == 0) {
    input->eof = 1;

    assert(MPEG_BUFSZ - input->length >= MAD_BUFFER_GUARD);

    while (len < MAD_BUFFER_GUARD)
      input->data[input->length + len++] = 0;
  }

  mad_stream_buffer(stream, input->data, input->length += len);

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode->header()
 * DESCRIPTION:	decide whether to continue decoding based on header
 */
static
enum mad_flow decode_header(void *data, struct mad_header const *header)
{
  struct player *player = data;

  if ((player->options & PLAYER_OPTION_TIMED) &&
      mad_timer_compare(player->stats.global_timer, player->global_stop) > 0)
    return MAD_FLOW_STOP;

  if (player->stats.absolute_framecount) {
    /* delay counting first frame */

    ++player->stats.absolute_framecount;
    mad_timer_add(&player->stats.absolute_timer, header->duration);

    ++player->stats.global_framecount;
    mad_timer_add(&player->stats.global_timer, header->duration);

    if ((player->options & PLAYER_OPTION_SKIP) &&
	mad_timer_compare(player->stats.global_timer,
			  player->global_start) < 0)
      return MAD_FLOW_IGNORE;
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode->filter()
 * DESCRIPTION:	perform filtering on decoded frame
 */
static
enum mad_flow decode_filter(void *data, struct mad_stream const *stream,
			    struct mad_frame *frame)
{
  struct player *player = data;

  if (player->stats.absolute_framecount == 0) {
    /* first frame */

    if (xing_parse(&player->input.xing,
		   stream->anc_ptr, stream->anc_bitlen) == 0) {
      if (player->input.xing.flags & XING_FRAMES) {
	player->stats.total_time = frame->header.duration;
	mad_timer_multiply(&player->stats.total_time,
			   player->input.xing.frames);
      }

      if (player->stats.total_bytes >= stream->next_frame - stream->this_frame)
	player->stats.total_bytes -= stream->next_frame - stream->this_frame;

      return MAD_FLOW_IGNORE;
    }

    ++player->stats.absolute_framecount;
    mad_timer_add(&player->stats.absolute_timer, frame->header.duration);

    ++player->stats.global_framecount;
    mad_timer_add(&player->stats.global_timer, frame->header.duration);

    if ((player->options & PLAYER_OPTION_SKIP) &&
	mad_timer_compare(player->stats.global_timer,
			  player->global_start) < 0)
      return MAD_FLOW_IGNORE;
  }

  return filter_run(player->output.filters, frame);
}

/*
 * NAME:	show_id3()
 * DESCRIPTION:	display an ID3 tag
 */
static
void show_id3(struct id3_tag const *tag)
{
  unsigned int i;
  struct id3_frame const *frame;
  id3_ucs4_t const *ucs4;
  id3_latin1_t *latin1;
  char const spaces[] = "          ";

  struct {
    char const *id;
    char const *name;
  } const info[] = {
    { ID3_FRAME_TITLE,  N_("Title")     },
    { "TIT3",           0               },  /* Subtitle */
    { "TCOP",           0,              },  /* Copyright */
    { "TPRO",           0,              },  /* Produced */
    { "TCOM",           N_("Composer")  },
    { ID3_FRAME_ARTIST, N_("Artist")    },
    { "TPE2",           N_("Orchestra") },
    { "TPE3",           N_("Conductor") },
    { "TEXT",           N_("Lyricist")  },
    { ID3_FRAME_ALBUM,  N_("Album")     },
    { ID3_FRAME_YEAR,   N_("Year")      },
    { ID3_FRAME_TRACK,  N_("Track")     },
    { "TPUB",           N_("Publisher") },
    { ID3_FRAME_GENRE,  N_("Genre")     },
    { "TRSN",           N_("Station")   },
    { "TENC",           N_("Encoder")   }
  };

  /* text information */

  for (i = 0; i < sizeof(info) / sizeof(info[0]); ++i) {
    union id3_field const *field;
    unsigned int nstrings, namelen, j;
    char const *name;

    frame = id3_tag_findframe(tag, info[i].id, 0);
    if (frame == 0)
      continue;

    field    = &frame->fields[1];
    nstrings = id3_field_getnstrings(field);

    name = info[i].name;
    if (name)
      name = gettext(name);

    namelen = name ? strlen(name) : 0;
    assert(namelen < sizeof(spaces));

    for (j = 0; j < nstrings; ++j) {
      ucs4 = id3_field_getstrings(field, j);
      assert(ucs4);

      if (strcmp(info[i].id, ID3_FRAME_GENRE) == 0)
	ucs4 = id3_genre_name(ucs4);

      latin1 = id3_ucs4_latin1duplicate(ucs4);
      if (latin1 == 0)
	goto fail;

      if (j == 0 && name)
	message("%s%s: %s\n", &spaces[namelen], name, latin1);
      else {
	if (strcmp(info[i].id, "TCOP") == 0 ||
	    strcmp(info[i].id, "TPRO") == 0) {
	  message("%s  %s %s\n", spaces, (info[i].id[1] == 'C') ?
		  _("Copyright (C)") : _("Produced (P)"), latin1);
	}
	else
	  message("%s  %s\n", spaces, latin1);
      }

      free(latin1);
    }
  }

  /* comments */

  i = 0;
  while ((frame = id3_tag_findframe(tag, ID3_FRAME_COMMENT, i++))) {
    id3_latin1_t *ptr, *newline;
    int first = 1;

    ucs4 = id3_field_getstring(&frame->fields[2]);
    assert(ucs4);

    if (*ucs4)
      continue;

    ucs4 = id3_field_getfullstring(&frame->fields[3]);
    assert(ucs4);

    latin1 = id3_ucs4_latin1duplicate(ucs4);
    if (latin1 == 0)
      goto fail;

    ptr = latin1;
    while (*ptr) {
      newline = strchr(ptr, '\n');
      if (newline)
	*newline = 0;

      if (strlen(ptr) > 66) {
	id3_latin1_t *linebreak;

	linebreak = ptr + 66;

	while (linebreak > ptr && *linebreak != ' ')
	  --linebreak;

	if (*linebreak == ' ') {
	  if (newline)
	    *newline = '\n';

	  newline = linebreak;
	  *newline = 0;
	}
      }

      if (first) {
	char const *name;
	unsigned int namelen;

	name    = _("Comment");
	namelen = strlen(name);
	assert(namelen < sizeof(spaces));

	message("%s%s: %s\n", &spaces[namelen], name, ptr);
	first = 0;
      }
      else 
	message("%s  %s\n", spaces, ptr);

      ptr += strlen(ptr) + (newline ? 1 : 0);
    }

    free(latin1);
    break;
  }

  if (0) {
  fail:
    error("id3", _("not enough memory to display tag"));
  }
}

/*
 * NAME:	show_status()
 * DESCRIPTION:	generate and output stream statistics
 */
static
void show_status(struct stats *stats,
		 struct mad_header const *header, char const *label, int now)
{
  signed long seconds;
  static char const *const layer_str[3] = { N_("I"), N_("II"), N_("III") };
  static char const *const mode_str[4] = {
    N_("single channel"), N_("dual channel"), N_("joint stereo"), N_("stereo")
  };

  if (header) {
    unsigned int bitrate;

    bitrate = header->bitrate / 1000;

    stats->vbr_rate += bitrate;
    stats->vbr_frames++;

    stats->vbr += (stats->bitrate && stats->bitrate != bitrate) ? 128 : -1;
    if (stats->vbr < 0)
      stats->vbr = 0;
    else if (stats->vbr > 512)
      stats->vbr = 512;

    stats->bitrate = bitrate;
  }

  seconds = mad_timer_count(stats->global_timer, MAD_UNITS_SECONDS);
  if (seconds != stats->nsecs || !on_same_line || now) {
    mad_timer_t timer;
    char time_str[18];
    char const *joint_str = "";

    stats->nsecs = seconds;

    switch (stats->show) {
    case STATS_SHOW_OVERALL:
      timer = stats->global_timer;
      break;

    case STATS_SHOW_CURRENT:
      timer = stats->absolute_timer;
      break;

    case STATS_SHOW_REMAINING:
      timer = stats->total_time;

      if (mad_timer_sign(timer) == 0 && stats->total_bytes) {
	unsigned long rate;

	/* estimate based on size and bitrate */

	rate = stats->vbr ?
	  stats->vbr_rate * 125 / stats->vbr_frames : stats->bitrate * 125UL;

	mad_timer_set(&timer, 0, stats->total_bytes, rate);
      }

      mad_timer_negate(&timer);
      mad_timer_add(&timer, stats->absolute_timer);
      break;
    }

    mad_timer_string(timer, time_str, " %02lu:%02u:%02u",
		     MAD_UNITS_HOURS, 0, 0);
    if (mad_timer_sign(timer) < 0)
      time_str[0] = '-';

    if (label || stats->label) {
      message("%s %s", time_str, label ? label : stats->label);
      stats->label = now ? label : 0;
    }
    else if (header) {
      if (header->mode == MAD_MODE_JOINT_STEREO) {
	switch (header->flags & (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO)) {
	case 0:
	  joint_str = _(" (LR)");
	  break;

	case MAD_FLAG_MS_STEREO:
	  joint_str = _(" (MS)");
	  break;

	case MAD_FLAG_I_STEREO:
	  joint_str = _(" (I)");
	  break;

	case (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO):
	  joint_str = _(" (MS+I)");
	  break;
	}
      }

      message(_("%s Layer %s, %s%u kbps%s, %u Hz, %s%s, %s"),
	      time_str, gettext(layer_str[header->layer - 1]),
	      stats->vbr ? _("VBR (avg ") : "",
	      stats->vbr ? ((stats->vbr_rate * 2) /
			    stats->vbr_frames + 1) / 2 : stats->bitrate,
	      stats->vbr ? _(")") : "",
	      header->samplerate, gettext(mode_str[header->mode]), joint_str,
	      (header->flags & MAD_FLAG_PROTECTION) ? _("CRC") : _("no CRC"));
    }
    else
      message("%s", time_str);
  }
}

/*
 * NAME:	decode->output()
 * DESCRIPTION: configure audio module and output decoded samples
 */
static
enum mad_flow decode_output(void *data, struct mad_header const *header,
			    struct mad_pcm *pcm)
{
  struct player *player = data;
  struct output *output = &player->output;
  mad_fixed_t const *ch1, *ch2;
  unsigned int nchannels;
  union audio_control control;

  ch1 = pcm->samples[0];
  ch2 = pcm->samples[1];

  switch (nchannels = pcm->channels) {
  case 1:
    ch2 = 0;
    if (output->select == PLAYER_CHANNEL_STEREO) {
      ch2 = ch1;
      nchannels = 2;
    }
    break;

  case 2:
    switch (output->select) {
    case PLAYER_CHANNEL_RIGHT:
      ch1 = ch2;
      /* fall through */

    case PLAYER_CHANNEL_LEFT:
      ch2 = 0;
      nchannels = 1;
      /* fall through */

    case PLAYER_CHANNEL_STEREO:
      break;

    default:
      if (header->mode == MAD_MODE_DUAL_CHANNEL) {
	if (output->select == PLAYER_CHANNEL_DEFAULT) {
	  if (player->verbosity >= -1) {
	    error("output",
		  _("no channel selected for dual channel; using first"));
	  }

	  output->select = -PLAYER_CHANNEL_LEFT;
	}

	ch2 = 0;
	nchannels = 1;
      }
    }
  }

  if (output->channels_in != nchannels ||
      output->speed_in != pcm->samplerate) {
    if (player->verbosity >= 1 &&
	pcm->samplerate != header->samplerate) {
      error("output", _("using output sample frequency %u Hz"),
	    pcm->samplerate);
    }

    audio_control_init(&control, AUDIO_COMMAND_CONFIG);

    control.config.channels  = nchannels;
    control.config.speed     = pcm->samplerate;
    control.config.precision = output->precision_in;

    if (output->command(&control) == -1) {
      error("output", audio_error);
      return MAD_FLOW_BREAK;
    }

    output->channels_in  = nchannels;
    output->speed_in     = pcm->samplerate;

    output->channels_out  = control.config.channels;
    output->speed_out     = control.config.speed;
    output->precision_out = control.config.precision;

    if (player->verbosity >= -1 &&
	output->channels_in != output->channels_out) {
      if (output->channels_in == 1)
	error("output", _("mono output not available; forcing stereo"));
      else {
	error("output", _("stereo output not available; using first channel "
			  "(use -m to mix)"));
      }
    }

    if (player->verbosity >= 0 &&
	output->speed_in != output->speed_out) {
      error("output", _("sample frequency %u Hz not available; closest %u Hz"),
	    output->speed_in, output->speed_out);
    }

    if (player->verbosity >= -1 &&
	output->precision_in &&
	output->precision_in != output->precision_out) {
      error("output", _("bit depth %u not available; using %u"),
	    output->precision_in, output->precision_out);
    }

    /* check whether resampling is necessary */
    if (abs(output->speed_out - output->speed_in) <
	(long) FREQ_TOLERANCE * output->speed_in / 100) {
      if (output->resampled) {
	resample_finish(&output->resample[0]);
	resample_finish(&output->resample[1]);

	free(output->resampled);
	output->resampled = 0;
      }
    }
    else {
      if (output->resampled) {
	resample_finish(&output->resample[0]);
	resample_finish(&output->resample[1]);
      }
      else {
	output->resampled = malloc(sizeof(*output->resampled));
	if (output->resampled == 0) {
	  error("output",
		_("not enough memory to allocate resampling buffer"));
	  return MAD_FLOW_BREAK;
	}
      }

      if (resample_init(&output->resample[0],
			output->speed_in, output->speed_out) == -1 ||
	  resample_init(&output->resample[1],
			output->speed_in, output->speed_out) == -1) {
	error("output", _("cannot resample %u Hz to %u Hz"),
	      output->speed_in, output->speed_out);

	free(output->resampled);
	output->resampled = 0;

	return MAD_FLOW_BREAK;
      }
      else if (player->verbosity >= -1) {
	error("output", _("resampling %u Hz to %u Hz"),
	      output->speed_in, output->speed_out);
      }
    }
  }

  audio_control_init(&control, AUDIO_COMMAND_PLAY);

  if (output->channels_in != output->channels_out)
    ch2 = (output->channels_out == 2) ? ch1 : 0;

  if (output->resampled) {
    control.play.nsamples = resample_block(&output->resample[0],
					   pcm->length, ch1,
					   (*output->resampled)[0]);
    control.play.samples[0] = (*output->resampled)[0];

    if (ch2 == ch1)
      control.play.samples[1] = control.play.samples[0];
    else if (ch2) {
      resample_block(&output->resample[1], pcm->length, ch2,
		     (*output->resampled)[1]);
      control.play.samples[1] = (*output->resampled)[1];
    }
    else
      control.play.samples[1] = 0;
  }
  else {
    control.play.nsamples   = pcm->length;
    control.play.samples[0] = ch1;
    control.play.samples[1] = ch2;
  }

  control.play.mode  = output->mode;
  control.play.stats = &player->stats.audio;

  if (output->command(&control) == -1) {
    error("output", audio_error);
    return MAD_FLOW_BREAK;
  }

  ++player->stats.play_framecount;
  mad_timer_add(&player->stats.play_timer, header->duration);

  if (player->verbosity > 0)
    show_status(&player->stats, header, 0, 0);

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	get_id3()
 * DESCRIPTION:	read and parse an ID3 tag from a stream
 */
static
struct id3_tag *get_id3(struct mad_stream *stream, id3_length_t tagsize,
			struct input *input)
{
  struct id3_tag *tag = 0;
  id3_length_t count;
  id3_byte_t const *data;
  id3_byte_t *allocated = 0;

  count = stream->bufend - stream->this_frame;

  if (tagsize <= count) {
    data = stream->this_frame;
    mad_stream_skip(stream, tagsize);
  }
  else {
    allocated = malloc(tagsize);
    if (allocated == 0) {
      error("id3", _("not enough memory to allocate tag data buffer"));
      goto fail;
    }

    memcpy(allocated, stream->this_frame, count);
    mad_stream_skip(stream, count);

    while (count < tagsize) {
      int len;

      do
	len = read(input->fd, allocated + count, tagsize - count);
      while (len == -1 && errno == EINTR);

      if (len == -1) {
	error("id3", ":read");
	goto fail;
      }

      if (len == 0) {
	error("id3", _("EOF while reading tag data"));
	goto fail;
      }

      count += len;
    }

    data = allocated;
  }

  tag = id3_tag_parse(data, tagsize);

 fail:
  if (allocated)
    free(allocated);

  return tag;
}

/*
 * NAME:	decode->error()
 * DESCRIPTION:	handle a decoding error
 */
static
enum mad_flow decode_error(void *data, struct mad_stream *stream,
			   struct mad_frame *frame)
{
  struct player *player = data;
  signed long tagsize;

  switch (stream->error) {
  case MAD_ERROR_BADDATAPTR:
    return MAD_FLOW_CONTINUE;

  case MAD_ERROR_LOSTSYNC:
    tagsize = id3_tag_query(stream->this_frame,
			    stream->bufend - stream->this_frame);
    if (tagsize > 0) {
      if (player->verbosity >= 0 &&
	  (player->options & PLAYER_OPTION_STREAMID3)) {
	struct id3_tag *tag;

	tag = get_id3(stream, tagsize, &player->input);
	if (tag) {
	  show_id3(tag);
	  id3_tag_delete(tag);
	}
      }
      else
	mad_stream_skip(stream, tagsize);

      if (player->stats.total_bytes >= tagsize)
	player->stats.total_bytes -= tagsize;

      return MAD_FLOW_CONTINUE;
    }

    /* fall through */

  default:
    if (player->verbosity >= -1 &&
	((stream->error == MAD_ERROR_LOSTSYNC && !player->input.eof)
	 || stream->sync) &&
	player->stats.global_framecount != player->stats.error_frame) {
      error("error", _("frame %lu: %s"),
	    player->stats.absolute_framecount, mad_stream_errorstr(stream));
      player->stats.error_frame = player->stats.global_framecount;
    }
  }

  if (stream->error == MAD_ERROR_BADCRC) {
    if (player->stats.global_framecount == player->stats.mute_frame)
      mad_frame_mute(frame);

    player->stats.mute_frame = player->stats.global_framecount + 1;

    return MAD_FLOW_IGNORE;
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode()
 * DESCRIPTION:	decode and output audio for an open file
 */
static
int decode(struct player *player)
{
  struct stat stat;
  struct mad_decoder decoder;
  int options, result;

  if (fstat(player->input.fd, &stat) == -1) {
    error("decode", ":fstat");
    return -1;
  }

  player->stats.total_bytes = S_ISREG(stat.st_mode) ? stat.st_size : 0;
  player->stats.total_time  = mad_timer_zero;

  xing_init(&player->input.xing);

  /* prepare input buffers */

# if defined(HAVE_MMAP)
  if (S_ISREG(stat.st_mode) && stat.st_size > 0) {
    player->input.length = stat.st_size;

    player->input.fdm = map_file(player->input.fd, player->input.length);
    if (player->input.fdm == 0 && player->verbosity >= 0)
      error("decode", ":mmap");

    player->input.data = player->input.fdm;
  }
# endif

  if (player->input.data == 0) {
    player->input.data = malloc(MPEG_BUFSZ);
    if (player->input.data == 0) {
      error("decode", _("not enough memory to allocate input buffer"));
      return -1;
    }

    player->input.length = 0;
  }

  player->input.eof = 0;

  /* reset statistics */
  player->stats.absolute_timer        = mad_timer_zero;
  player->stats.play_timer            = mad_timer_zero;
  player->stats.absolute_framecount   = 0;
  player->stats.play_framecount       = 0;
  player->stats.error_frame           = -1;
  player->stats.vbr                   = 0;
  player->stats.bitrate               = 0;
  player->stats.vbr_frames            = 0;
  player->stats.vbr_rate              = 0;
  player->stats.audio.clipped_samples = 0;
  player->stats.audio.peak_clipping   = 0;
  player->stats.audio.peak_sample     = 0;

  mad_decoder_init(&decoder, player,
# if defined(HAVE_MMAP)
		   player->input.fdm ? decode_input_mmap :
# endif
		   decode_input_read,
		   decode_header, decode_filter,
		   player->output.command ? decode_output : 0,
		   decode_error, 0);

  options = 0;
  if (player->options & PLAYER_OPTION_DOWNSAMPLE)
    options |= MAD_OPTION_HALFSAMPLERATE;
  if (player->options & PLAYER_OPTION_IGNORECRC)
    options |= MAD_OPTION_IGNORECRC;

  mad_decoder_options(&decoder, options);

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  mad_decoder_finish(&decoder);

# if defined(HAVE_MMAP)
  if (player->input.fdm) {
    if (unmap_file(player->input.fdm, player->input.length) == -1) {
      error("decode", ":munmap");
      result = -1;
    }

    player->input.fdm = 0;

    if (!player->input.eof)
      player->input.data = 0;
  }
# endif

  if (player->input.data) {
    free(player->input.data);
    player->input.data = 0;
  }

  xing_finish(&player->input.xing);

  return result;
}

/*
 * NAME:	play_one()
 * DESCRIPTION:	open and play a single file
 */
static
int play_one(struct player *player)
{
  char const *file = player->playlist.entries[player->playlist.current];
  int result;

  if (strcmp(file, "-") == 0) {
    if (isatty(STDIN_FILENO)) {
      error(0, "%s: %s", _("stdin"), _("is a tty"));
      return -1;
    }

    player->input.path = _("stdin");
    player->input.fd   = STDIN_FILENO;
  }
  else {
    player->input.path = file;
    player->input.fd   = open(player->input.path, O_RDONLY | O_BINARY);
    if (player->input.fd == -1) {
      error(0, ":", player->input.path);
      return -1;
    }
  }

  player->options &= ~PLAYER_OPTION_STREAMID3;

  if (player->verbosity >= 0) {
    int fd;
    struct id3_file *file;

    if (player->playlist.length > 1)
      message(">> %s\n", player->input.path);

    fd = dup(player->input.fd);
    file = id3_file_fdopen(fd, ID3_FILE_MODE_READONLY);
    if (file == 0) {
      close(fd);
      player->options |= PLAYER_OPTION_STREAMID3;
    }
    else {
      show_id3(id3_file_tag(file));
      id3_file_close(file);
    }
  }

  result = decode(player);

  if (result == 0 && player->verbosity >= 0) {
    char time_str[19], db_str[7];
    char const *peak_str;
    mad_fixed_t peak;

    mad_timer_string(player->stats.play_timer, time_str, "%lu:%02u:%02u.%1u",
		     MAD_UNITS_HOURS, MAD_UNITS_DECISECONDS, 0);

# if defined(HAVE_LOCALECONV)
    {
      char *point;

      point = strchr(time_str, '.');
      if (point)
	*point = *localeconv()->decimal_point;
    }
# endif

    peak = MAD_F_ONE + player->stats.audio.peak_clipping;
    if (peak == MAD_F_ONE)
      peak = player->stats.audio.peak_sample;

    if (peak == 0)
      peak_str = "-inf";
    else {
      sprintf(db_str, "%+.1f", 20 * log10(mad_f_todouble(peak)));
      peak_str = db_str;
    }

    message(_("%lu frames decoded (%s), "
	      "%s dB peak amplitude, "
	      "%lu clipped samples\n"),
	    player->stats.play_framecount, time_str, peak_str,
	    player->stats.audio.clipped_samples);
  }

  if (player->input.fd != STDIN_FILENO &&
      close(player->input.fd) == -1 && result == 0) {
    error(0, ":", player->input.path);
    result = -1;
  }

  return result;
}

/*
 * NAME:	play_all()
 * DESCRIPTION:	run the player's playlist
 */
static
int play_all(struct player *player)
{
  int count, i, j, result = 0;
  struct playlist *playlist = &player->playlist;
  char const *tmp;

  /* set up playlist */

  count = playlist->length;

  if (player->options & PLAYER_OPTION_SHUFFLE) {
    srand(time(0));

    /* initial shuffle */
    for (i = 0; i < count; ++i) {
      j = rand() % count;

      tmp = playlist->entries[i];
      playlist->entries[i] = playlist->entries[j];
      playlist->entries[j] = tmp;
    }
  }

  /* run playlist */

  while (count && (player->repeat == -1 || player->repeat--)) {
    while (playlist->current < playlist->length) {
      i = playlist->current;

      if (playlist->entries[i] == 0) {
	++playlist->current;
	continue;
      }

      player->control = PLAYER_CONTROL_DEFAULT;

      if (play_one(player) == -1) {
	playlist->entries[i] = 0;
	--count;

	result = -1;
      }

      if ((player->options & PLAYER_OPTION_TIMED) &&
	  mad_timer_compare(player->stats.global_timer,
			    player->global_stop) > 0) {
	count = 0;
	break;
      }

      switch (player->control) {
      case PLAYER_CONTROL_DEFAULT:
	if ((player->options & PLAYER_OPTION_SHUFFLE) && player->repeat &&
	    ++i < playlist->length) {
	  /* pick something from the next half only */
	  j = (i + rand() % ((playlist->length + 1) / 2)) % playlist->length;

	  tmp = playlist->entries[i];
	  playlist->entries[i] = playlist->entries[j];
	  playlist->entries[j] = tmp;
	}
	/* fall through */

      case PLAYER_CONTROL_NEXT:
	++playlist->current;
	break;

      case PLAYER_CONTROL_PREVIOUS:
	do {
	  if (playlist->current-- == 0)
	    playlist->current = playlist->length;
	}
	while (playlist->current < playlist->length &&
	       playlist->entries[playlist->current] == 0);
	break;

      case PLAYER_CONTROL_REPLAY:
	break;

      case PLAYER_CONTROL_STOP:
	playlist->current = playlist->length;
	count = 0;
	break;
      }
    }

    playlist->current = 0;
  }

  return result;
}

/*
 * NAME:	stop_audio()
 * DESCRIPTION:	stop playing the audio device immediately
 */
static
int stop_audio(struct player *player, int flush)
{
  int result = 0;

  if (player->output.command) {
    union audio_control control;

    audio_control_init(&control, AUDIO_COMMAND_STOP);
    control.stop.flush = flush;

    result = player->output.command(&control);
  }

  return result;
}

# if defined(USE_TTY)
/*
 * NAME:	readkey()
 * DESCRIPTION:	read a keypress from the keyboard
 */
static
int readkey(int blocking)
{
# if !defined(_WIN32)
  unsigned char key;
  ssize_t count;

  if (!blocking) {
    /* tty_fd should be a tty in noncanonical mode with VMIN = VTIME = 0 */

    count = read(tty_fd, &key, 1);
    if (count == -1 && errno != EINTR) {
      error("tty", ":read");
      return -1;
    }

    return (count == 1) ? key : 0;
  }
  else {
    struct termios tty, save_tty;

    if (tcgetattr(tty_fd, &tty) == -1) {
      error("tty", ":tcgetattr");
      return -1;
    }

    save_tty = tty;

    /* change terminal temporarily to get a blocking read() */

    tty.c_cc[VMIN] = 1;

    if (tcsetattr(tty_fd, TCSANOW, &tty) == -1) {
      error("tty", ":tcsetattr");
      return -1;
    }

    do
      count = read(tty_fd, &key, 1);
    while (count == -1 && errno == EINTR);

    if (count == -1)
      error("tty", ":read");

    if (tcsetattr(tty_fd, TCSANOW, &save_tty) == -1) {
      error("tty", ":tcsetattr");
      return -1;
    }

    if (count == -1)
      return -1;

    return (count == 1) ? key : 0;
  }
# elif defined(_WIN32)
  HANDLE console;
  INPUT_RECORD input;
  DWORD count;

  console = GetStdHandle(STD_INPUT_HANDLE);

  do {
    if (GetNumberOfConsoleInputEvents(console, &count) == 0) {
      error("tty", "GetNumberOfConsoleInputEvents() failed");
      return -1;
    }

    if (count == 0) {
      if (!blocking)
	return 0;
      else {
	/* this is necessary to keep Windows from hanging (!) */
	Sleep(500);

	switch (WaitForSingleObject(console, INFINITE)) {
	case WAIT_ABANDONED:
	case WAIT_OBJECT_0:
	  continue;

	case WAIT_TIMEOUT:
	default:
	  /* ? */
	case WAIT_FAILED:
	  error("tty", "WaitForSingleObject() failed");
	  return -1;
	}
      }
    }

    if (ReadConsoleInput(console, &input, 1, &count) == 0 || count != 1) {
      error("tty", "ReadConsoleInput() failed");
      return -1;
    }
  }
  while (input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown ||
	 input.Event.KeyEvent.uChar.AsciiChar == 0);

  return (unsigned char) input.Event.KeyEvent.uChar.AsciiChar;
# endif

  return blocking ? -1 : 0;
}

/*
 * NAME:	tty_filter()
 * DESCRIPTION:	process TTY commands
 */
static
enum mad_flow tty_filter(void *data, struct mad_frame *frame)
{
  struct player *player = data;
  enum mad_flow flow = MAD_FLOW_CONTINUE;
  int command, stopped = 0;

  command = readkey(0);
  if (command == -1)
    return MAD_FLOW_BREAK;

 again:
  switch (command) {
  case KEY_PAUSE:
  case KEY_STOP:
    stop_audio(player, stopped = (command == KEY_STOP));
    message(" --%s--", stopped ? _("Stopped") : _("Paused"));

    if (stopped) {
      player->control = PLAYER_CONTROL_REPLAY;
      flow = MAD_FLOW_STOP;
    }

    command = readkey(1);

    message("");

    if (command == -1)
      return MAD_FLOW_BREAK;

    if (command > 0 && command != KEY_PAUSE)
      goto again;

    break;

  case KEY_FORWARD:
  case KEY_CTRL('n'):
    player->control = PLAYER_CONTROL_NEXT;
    goto stop;

  case KEY_BACK:
  case KEY_CTRL('p'):
    {
      mad_timer_t threshold;

      mad_timer_set(&threshold, 4, 0, 0);

      player->control =
	(stopped ||
	 mad_timer_compare(player->stats.play_timer, threshold) < 0) ?
	PLAYER_CONTROL_PREVIOUS : PLAYER_CONTROL_REPLAY;
    }
    goto stop;

  case KEY_QUIT:
  case KEY_CTRL('c'):
  case 'Q':
    player->control = PLAYER_CONTROL_STOP;
    goto stop;

  case KEY_INFO:
  case '?':
    if (player->verbosity <= 0) {
      show_status(&player->stats, 0, player->input.path, 1);
      message("\n");
    }
    break;

  case KEY_TIME:
    if (player->verbosity > 0) {
      char const *label = 0;

      switch (player->stats.show) {
      case STATS_SHOW_CURRENT:
	if (player->playlist.length > 1) {
	  player->stats.show = STATS_SHOW_OVERALL;
	  label = N_("[Overall Time]");
	  break;
	}
	/* else fall through */

      case STATS_SHOW_OVERALL:
	player->stats.show = STATS_SHOW_REMAINING;
	label = N_("[Current Time Remaining]");
	break;

      case STATS_SHOW_REMAINING:
	player->stats.show = STATS_SHOW_CURRENT;
	label = N_("[Current Time]");
	break;
      }

      show_status(&player->stats, 0, gettext(label), 1);
    }
    break;
  }

  return flow;

 stop:
  stop_audio(player, 1);
  return MAD_FLOW_STOP;
}
# endif

/*
 * NAME:	addfilter()
 * DESCRIPTION:	insert a filter at the beginning of the filter chain
 */
static
int addfilter(struct player *player, filter_func_t *func, void *data)
{
  struct filter *filter;

  filter = filter_new(func, data, player->output.filters);
  if (filter == 0)
    return -1;

  player->output.filters = filter;

  return 0;
}

/*
 * NAME:	setup_filters()
 * DESCRIPTION:	create output filters
 */
static
int setup_filters(struct player *player)
{
  static struct equalizer attenuation;

  /* filters must be added in reverse order */

# if defined(EXPERIMENTAL)
  {
    if ((player->options & PLAYER_OPTION_EXTERNALMIX) &&
	addfilter(player, mixer_filter, stdout) == -1)
      return -1;

    if ((player->options & PLAYER_OPTION_EXPERIMENTAL) &&
	addfilter(player, experimental_filter, 0) == -1)
      return -1;
  }
# endif

  if ((player->options & PLAYER_OPTION_FADEIN) &&
      addfilter(player, fadein_filter, player) == -1)
    return -1;

  if (player->output.attenuation != MAD_F_ONE) {
    int sb;

    equalizer_init(&attenuation);

    for (sb = 0; sb < 32; ++sb) {
      attenuation.bandfactors[0][sb] =
      attenuation.bandfactors[1][sb] = player->output.attenuation;
    }

    if (addfilter(player, equalizer_filter, &attenuation) == -1)
      return -1;
  }

  if (player->output.select == PLAYER_CHANNEL_MONO &&
      addfilter(player, mono_filter, player) == -1)
    return -1;

# if defined(USE_TTY)
  if ((player->options & PLAYER_OPTION_TTYCONTROL) &&
      addfilter(player, tty_filter, player) == -1)
    return -1;
# endif

  return 0;
}

# if defined(USE_TTY) && !defined(_WIN32)
/*
 * NAME:	restore_tty()
 * DESCRIPTION:	revert to previous terminal settings
 */
static
int restore_tty(int interrupt)
{
  struct termios tty;
  struct sigaction action;
  int result = 0;

  if (tcgetattr(tty_fd, &tty) == 0 &&
      tcsetattr(tty_fd, interrupt ? TCSAFLUSH : TCSADRAIN,
		&save_tty) == -1) {
    if (!interrupt)
      error("tty", ":tcsetattr");
    result = -1;
  }

  save_tty = tty;

  if (sigaction(SIGINT, 0, &action) == 0 &&
      sigaction(SIGINT, &save_sigint, 0) == -1) {
    if (!interrupt)
      error("tty", ":sigaction(SIGINT)");
    result = -1;
  }

  save_sigint = action;

  if (sigaction(SIGTSTP, 0, &action) == 0 &&
      sigaction(SIGTSTP, &save_sigtstp, 0) == -1) {
    if (!interrupt)
      error("tty", ":sigaction(SIGTSTP)");
    result = -1;
  }

  save_sigtstp = action;

  if (!interrupt) {
    if (close(tty_fd) == -1) {
      error("tty", ":close");
      result = -1;
    }

    tty_fd = -1;
  }

  return result;
}

/*
 * NAME:	signal_handler()
 * DESCRIPTION:	restore tty state after software interrupt
 */
static
void signal_handler(int signal)
{
  static struct sigaction save_sigcont;

  /* restore tty state and previous signal actions */

  restore_tty(1);

  /* handle SIGCONT after SIGTSTP */

  switch (signal) {
  case SIGTSTP:
    {
      struct sigaction action;

      sigaction(SIGCONT, 0, &save_sigcont);

      action = save_sigcont;
      action.sa_handler = signal_handler;
      sigemptyset(&action.sa_mask);
      action.sa_flags = 0;

      sigaction(SIGCONT, &action, 0);
    }
    break;

  case SIGCONT:
    sigaction(SIGCONT, &save_sigcont, 0);
    break;
  }

  /* re-send signal, which is currently blocked */

  kill(getpid(), signal);

  /* return to previous thread, which should immediately receive the signal */

  return;
}

/*
 * NAME:	setup_tty()
 * DESCRIPTION:	change terminal parameters and signal handlers
 */
static
int setup_tty(void)
{
  struct termios tty;
  struct sigaction action;

  /* open controlling terminal */

  tty_fd = open(TTY_DEVICE, O_RDONLY);
  if (tty_fd == -1) {
    error("tty", ":", TTY_DEVICE);
    return -1;
  }

  /* save current terminal and signal settings */

  if (tcgetattr(tty_fd, &save_tty) == -1) {
    error("tty", ":tcgetattr");
    return -1;
  }

  if (sigaction(SIGTSTP, 0, &save_sigtstp) == -1) {
    error("tty", ":sigaction(SIGTSTP)");
    return -1;
  }

  if (sigaction(SIGINT, 0, &save_sigint) == -1) {
    error("tty", ":sigaction(SIGINT)");
    return -1;
  }

  /* catch SIGTSTP and SIGINT so the tty state can be restored */

  action = save_sigtstp;
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGTSTP, &action, 0) == -1) {
    error("tty", ":sigaction(SIGTSTP)");
    goto fail;
  }

  action = save_sigint;
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGINT, &action, 0) == -1) {
    error("tty", ":sigaction(SIGINT)");
    goto fail;
  }

  /* turn off echo and canonical mode */

  tty = save_tty;

  tty.c_lflag &= ~(ECHO | ICANON);

  /* set VMIN = VTIME = 0 so read() always returns immediately */

  tty.c_cc[VMIN]  = 0;
  tty.c_cc[VTIME] = 0;

  if (tcsetattr(tty_fd, TCSAFLUSH, &tty) == -1) {
    error("tty", ":tcsetattr");
    goto fail;
  }

  return 0;

 fail:
  sigaction(SIGINT,  &save_sigint,  0);
  sigaction(SIGTSTP, &save_sigtstp, 0);
  return -1;
}
# endif

/*
 * NAME:	silence()
 * DESCRIPTION:	output silence for a period of time
 */
static
int silence(struct player *player, mad_timer_t duration, char const *label)
{
  union audio_control control;
  unsigned int nchannels, speed, nsamples;
  mad_fixed_t *samples;
  mad_timer_t unit;
  int result = 0;

  audio_control_init(&control, AUDIO_COMMAND_CONFIG);
  control.config.channels = 2;
  control.config.speed    = 44100;

  if (player->output.command(&control) == -1) {
    error("audio", audio_error);
    return -1;
  }

  nchannels = control.config.channels;
  speed     = control.config.speed;
  nsamples  = speed > MAX_NSAMPLES ? MAX_NSAMPLES : speed;

  player->output.channels_in  = nchannels;
  player->output.channels_out = nchannels;
  player->output.speed_in     = speed;
  player->output.speed_out    = speed;

  samples = calloc(nsamples, sizeof(mad_fixed_t));
  if (samples == 0) {
    error("silence", _("not enough memory to allocate sample buffer"));
    return -1;
  }

  audio_control_init(&control, AUDIO_COMMAND_PLAY);
  control.play.nsamples   = nsamples;
  control.play.samples[0] = samples;
  control.play.samples[1] = (nchannels == 2) ? samples : 0;
  control.play.mode       = player->output.mode;
  control.play.stats      = &player->stats.audio;

  mad_timer_set(&unit, 0, nsamples, speed);

  for (mad_timer_negate(&duration);
       mad_timer_sign(duration) < 0;
       mad_timer_add(&duration, unit)) {
    if (mad_timer_compare(unit, mad_timer_abs(duration)) > 0) {
      unit = mad_timer_abs(duration);
      control.play.nsamples = mad_timer_fraction(unit, speed);
    }

    if (player->output.command(&control) == -1) {
      error("audio", audio_error);
      goto fail;
    }

    mad_timer_add(&player->stats.global_timer, unit);

    if (player->verbosity > 0)
      show_status(&player->stats, 0, label, 0);
  }

  if (0) {
  fail:
    result = -1;
  }

  free(samples);

  return result;
}

/*
 * NAME:	player->run()
 * DESCRIPTION:	begin playback
 */
int player_run(struct player *player, int argc, char const *argv[])
{
  int result = 0;
  union audio_control control;

  player->playlist.entries = argv;
  player->playlist.length  = argc;

  /* set up terminal settings */

# if defined(USE_TTY) && !defined(_WIN32)
  if ((player->options & PLAYER_OPTION_TTYCONTROL) && setup_tty() == -1)
    player->options &= ~PLAYER_OPTION_TTYCONTROL;
# endif

  /* set up filters */

  if (setup_filters(player) == -1) {
    error("filter", _("not enough memory to allocate filters"));
    goto fail;
  }

  /* initialize audio */

  if (player->output.command) {
    audio_control_init(&control, AUDIO_COMMAND_INIT);
    control.init.path = player->output.path;

    if (player->output.command(&control) == -1) {
      error("audio", audio_error, control.init.path);
      goto fail;
    }

    if ((player->options & PLAYER_OPTION_SKIP) &&
	mad_timer_sign(player->global_start) < 0) {
      player->stats.global_timer = player->global_start;

      if (silence(player, mad_timer_abs(player->global_start),
		  _("lead-in")) == -1)
	goto fail;
    }
  }

  /* run playlist */

  result = play_all(player);

  /* drain and close audio */

  if (player->output.command) {
    audio_control_init(&control, AUDIO_COMMAND_FINISH);

    if (player->output.command(&control) == -1) {
      error("audio", audio_error);
      goto fail;
    }
  }

  if (0) {
  fail:
    result = -1;
  }

  /* restore terminal settings */

# if defined(USE_TTY) && !defined(_WIN32)
  if (player->options & PLAYER_OPTION_TTYCONTROL)
    restore_tty(0);
# endif

  return result;
}

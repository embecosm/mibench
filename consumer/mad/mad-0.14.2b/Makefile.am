##
## mad - MPEG audio decoder
## Copyright (C) 2000-2001 Robert Leslie
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## If you would like to negotiate alternate licensing terms, you may do
## so by contacting the author: Robert Leslie <rob@mars.org>
##
## $Id: Makefile.am,v 1.22 2001/11/09 00:40:38 rob Exp $
##

## Process this file with automake to produce Makefile.in

if BUILD_WINAMP
winamp = winamp
else
winamp =
endif

if BUILD_LIBZ
libz = libz
ldadd_libz = -Llibz -lz
else
libz =
ldadd_libz = -lz
endif

SUBDIRS =		intl po $(libz) libid3tag libmad $(winamp)
DIST_SUBDIRS =		intl po   libz  libid3tag libmad   winamp  msvc++

bin_PROGRAMS =		madplay
bin_SCRIPTS =		abxtest
EXTRA_PROGRAMS =	madtime minimad madmix mad123

man_MANS =		madplay.1 abxtest.1

EXTRA_DIST =  \
	$(man_MANS) $(bin_SCRIPTS)  \
	CHANGES COPYRIGHT CREDITS README TODO VERSION

default_audio =  \
	audio.c audio.h  \
	audio_cdda.c audio_aiff.c audio_wave.c audio_snd.c audio_raw.c  \
	audio_null.c

extra_audio =  \
	audio_oss.c audio_empeg.c audio_sun.c audio_win32.c  \
	audio_hex.c audio_esd.c

common_sources =  \
	version.c version.h global.h

madplay_SOURCES =  \
	$(common_sources) $(default_audio) madplay.c  \
	resample.c filter.c equalizer.c xing.c player.c  \
	resample.h filter.h equalizer.h xing.h player.h  \
	getopt.c getopt1.c getopt.h

EXTRA_madplay_SOURCES =  \
	$(extra_audio)

madtime_SOURCES =	$(common_sources) madtime.c
minimad_SOURCES =	minimad.c
mad123_SOURCES =	mad123.c

madmix_SOURCES =	$(common_sources) madmix.c $(default_audio)
EXTRA_madmix_SOURCES =	$(extra_audio)

audio =			@AUDIO@

localedir =		$(datadir)/locale
INCLUDES =		-Ilibmad -I$(srcdir)/libid3tag -Iintl  \
			-DLOCALEDIR=\"$(localedir)\"

depend_libmad =		libmad/libmad.la
depend_libid3tag =	libid3tag/libid3tag.la

ldadd_libmad =		$(depend_libmad)
ldadd_libid3tag =	$(depend_libid3tag) $(ldadd_libz)

depend =		@LIBOBJS@ @INTLLIBS@
ldadd =			$(depend)

DEPENDENCIES =		$(depend) $(depend_libmad)
LDADD =			$(ldadd)  $(ldadd_libmad)

madplay_DEPENDENCIES =	$(audio) $(DEPENDENCIES) $(depend_libid3tag)
madplay_LDADD =		$(audio) $(LDADD)        $(ldadd_libid3tag)

madmix_DEPENDENCIES =	$(audio) $(DEPENDENCIES)
madmix_LDADD =		$(audio) $(LDADD)

CLEANFILES =		$(EXTRA_PROGRAMS) *.exe gmon.out

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

profile: madplay gmon.out
	gprof madplay | less

gmon.out: madplay test/profile.mpg
	./madplay -vo pcm:/dev/null test/profile.mpg || rm -f gmon.out

.c.s:
	$(COMPILE) -S $<

again: clean all

.PHONY: profile again

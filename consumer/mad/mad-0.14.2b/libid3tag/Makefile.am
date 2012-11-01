##
## libid3tag - ID3 tag manipulation library
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
## $Id: Makefile.am,v 1.13 2001/11/09 04:55:21 rob Exp $
##

## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES =	libid3tag.la
include_HEADERS =	id3tag.h

version_current =	1
version_revision =	0
version_age =		1

version_info =		$(version_current):$(version_revision):$(version_age)

EXTRA_DIST =		COPYRIGHT genre.dat.sed

if DEBUG
debug = debug.c debug.h
else
debug =
endif

libid3tag_la_SOURCES =	version.c ucs4.c latin1.c utf16.c utf8.c  \
			parse.c render.c field.c frametype.c compat.c  \
			genre.c frame.c crc.c util.c tag.c file.c  \
			version.h ucs4.h latin1.h utf16.h utf8.h  \
			parse.h render.h field.h frametype.h compat.h  \
			genre.h frame.h crc.h util.h tag.h file.h  \
			id3tag.h global.h genre.dat $(debug)

EXTRA_libid3tag_la_SOURCES =  \
			frametype.gperf compat.gperf genre.dat.in  \
			debug.c debug.h

libid3tag_la_LDFLAGS =	-version-info $(version_info)

BUILT_SOURCES =		frametype.c compat.c genre.dat

$(srcdir)/frametype.c: $(srcdir)/frametype.gperf Makefile.am
	gperf -tCcTonD -K id -N id3_frametype_lookup -s -3 -k '*' $< |  \
	sed -e '/\$$''Id: /s/\$$//g' >$@

$(srcdir)/compat.c: $(srcdir)/compat.gperf Makefile.am
	gperf -tCcTonD -K id -N id3_compat_lookup -s -3 -k '*' $< |  \
	sed -e '/\$$''Id: /s/\$$//g' >$@

$(srcdir)/genre.dat: $(srcdir)/genre.dat.in $(srcdir)/genre.dat.sed Makefile.am
	sed -n -f $(srcdir)/genre.dat.sed $< |  \
	sed -e '/\$$''Id: /s/\$$//g' >$@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

again: clean all

.PHONY: again

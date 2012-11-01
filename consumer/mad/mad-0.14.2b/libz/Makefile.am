##
## libz - zlib compression library
##

## Process this file with automake to produce Makefile.in

noinst_LIBRARIES =	libz.a
noinst_HEADERS =	zlib.h

EXTRA_PROGRAMS =	maketree

libz_a_SOURCES =	adler32.c compress.c crc32.c gzio.c inflate.c  \
			uncompr.c  \
			deflate.c infblock.c infcodes.c inffast.c inftrees.c  \
			infutil.c trees.c zutil.c  \
			deflate.h infblock.h infcodes.h inffast.h inftrees.h  \
			infutil.h trees.h zutil.h  \
			inffixed.h zconf.h zlib.h

maketree_SOURCES =	maketree.c
maketree_LDADD =	libz.a

EXTRA_DIST =		README ChangeLog

again: clean all

.PHONY: again

#
# $Id: Makefile,v 1.11 1994/08/31 05:58:47 geoff Exp $
#
# Copyright 1993, Geoff Kuenning, Granada Hills, CA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All modifications to the source code must be clearly marked as
#    such.  Binary redistributions based on modified source code
#    must be clearly marked as modified versions in the documentation
#    and/or other materials provided with the distribution.
# 4. All advertising materials mentioning features or use of this software
#    must display the following acknowledgment:
#      This product includes software developed by Geoff Kuenning and
#      other unpaid contributors.
# 5. The name of Geoff Kuenning may not be used to endorse or promote
#    products derived from this software without specific prior
#    written permission.
#
# THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# This makefile is an example of how you might write a makefile for a
# simple language which has only a single dictionary available.  For
# an example of a complex makefile, look at the makefile for English.
#
# $Log: Makefile,v $
# Revision 1.11  1994/08/31  05:58:47  geoff
# Create directories before installing into them.
#
# Revision 1.10  1994/05/25  04:29:34  geoff
# Purge Martin Schulz's obsolete e-mail address from the file.
#
# Revision 1.9  1994/02/22  06:09:08  geoff
# Add SHELLDEBUG.
#
# Revision 1.8  1994/02/07  06:18:03  geoff
# Use MAKE_SORTTMP when building the dictionary
#
# Revision 1.7  1994/02/07  06:07:57  geoff
# Add a dummy else clause to shell if-test for Ultrix
#
# Revision 1.6  1994/01/25  07:12:30  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#

SHELL = /bin/sh
MAKE = make

CONFIG		=	../../config.sh
PATHADDER	=	../..
BUILDHASH	=	../../buildhash
UNSQ		=	../../unsq
FIX8BIT		=	../fix8bit

# The following variables should be set by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
# The German dictionary I use is due to Martin Schulz, Moncton, New
# Brunswick, Canada.  It is not distributed with ispell due to its
# size, but is available for ftp (see the languages/Where file for
# locations).
#
# Martin's dictionary has been broken up into a number of sub-files
# which can be combined to make a master dictionary.  See the files
# README or LIESMICH for more information on what these sub-files
# contain.
#
# The DICTOPTIONS variable should be set to a list of one or more files,
# separated by spaces, and selected from the following options:
#
#	abkuerz.txt abweichend.txt compeng.txt elektronik.txt
#	geographie.txt infoabk.txt informatik.txt namen.txt
#	seltenes.txt technik.txt zusammen.txt
#
#
# If you change DICTOPTIONS in your local.h file, you will have to do
# "make dictclean" to clear out the old dictionary before you re-make.
#
DICTALWAYS	=	adjektive.txt worte.txt verben.txt
DICTOPTIONS	=	Use_LANGUAGES_from_config.X

#
# The following variables may be overridden by the superior Makefile,
# based on the LANGUAGES variable in config.X.  Note that selection of
# the affix file is closely related to the dictionary chosen; don't
# change the affix file unless you know what you are doing!
#
AFFIXES	=	deutsch.aff

#
#	Set this to "-vx" in the make command line if you need to
#	debug the complex shell commands.
#
SHELLDEBUG = +vx

all:	deutsch.hash

install: all $(CONFIG)
	@. $(CONFIG); \
	  set -x; \
	  [ -d $$LIBDIR ]  ||  (mkdir $$LIBDIR; chmod 755 $$LIBDIR); \
	  cd $$LIBDIR; rm -f deutsch.aff deutsch.hash
	@. $(CONFIG); \
	  set -x; \
	  cp deutsch.aff deutsch.hash $$LIBDIR
	@. $(CONFIG); \
	  set -x; \
	  cd $$LIBDIR; \
	  chmod 644 deutsch.aff deutsch.hash

deutsch.hash:	$(BUILDHASH) $(AFFIXES) deutsch.dict
	rm -f deutsch.hash
	$(BUILDHASH) deutsch.dict $(AFFIXES) deutsch.hash

$(AFFIXES):	deutsch.7bit $(FIX8BIT)
	$(FIX8BIT) -8 < deutsch.7bit > $(AFFIXES)

deutsch-alt.aff:	deutsch-alt.7bit $(FIX8BIT)
	$(FIX8BIT) -8 < deutsch-alt.7bit > deutsch-alt.aff

$(FIX8BIT):	../fix8bit.c
	cd ..; $(MAKE) fix8bit

deutsch.dict:	$(DICTALWAYS) $(DICTOPTIONS)
	. $(CONFIG); \
	  eval sort -f -o deutsch.dict $$MAKE_SORTTMP \
	    $(DICTALWAYS) $(DICTOPTIONS)

#
#	The following dependency can be executed when ispell is unpacked,
#	to unpack the dictionaries.
#
unpack:	$(AFFIXES)

$(UNSQ):
	set +e; \
	set $(SHELLDEBUG); \
	if [ ! -x $(UNSQ) ]; \
	then \
	    set -e; \
	    cd ../..; \
	    $(MAKE) unsq; \
	else \
	    : ; \
	fi

clean:
	rm -f core *.hash *.stat *.cnt

#
#	The following target is used in the English makefile, and is
#	required to be present in all other language Makefiles as
#	well, even though it doesn't have to do anything in those
#	directories.
#
kitclean:

#
#	The following target allows you to clean out the combined
#	dictionary file.
#
dictclean:
	rm -f deutsch.dict

#	required to be present in all other language Makefiles as
#	well, even though it doesn't have to do anything in those
#	directories.
#
dictclean:

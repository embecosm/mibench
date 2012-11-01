#
# $Id: Makefile,v 1.1 1994/12/28 01:03:54 geoff Exp $
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
# Revision 1.1  1994/12/28  01:03:54  geoff
# Initial revision
#
# Revision 1.6  1994/02/07  06:07:57  geoff
# Add a dummy else clause to shell if-test for Ultrix
#
# Revision 1.5  1994/01/25  07:12:44  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#

SHELL = /bin/sh
MAKE = make

PATHADDER	=	../..
BUILDHASH	=	buildhash
UNSQ		=	unsq
FIX8BIT		=	fix8bit

#
# The following variables make it easy to adapt this Makefile to
# numerous languages.
#
LANGUAGE	=	espanol
DICTIONARY	=	$(LANGUAGE).words
HASHFILE	=	$(LANGUAGE).hash

#
# The following variables may be overridden by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
AFFIXES	=	./$(LANGUAGE).aff

all:	$(HASHFILE)

$(HASHFILE):	$(AFFIXES) $(DICTIONARY)
	rm -f $(HASHFILE)
	munchlist -v -l $(AFFIXES) $(DICTIONARY) > $(LANGUAGE).words+
	$(BUILDHASH) $(LANGUAGE).words+ $(AFFIXES) $(HASHFILE)

build:
	$(BUILDHASH) -s $(LANGUAGE).words+ $(AFFIXES) $(HASHFILE)

$(LANGUAGE)-alt.aff:	$(LANGUAGE)-alt.7bit $(FIX8BIT)
	$(FIX8BIT) -8 < $(LANGUAGE)-alt.7bit > $(LANGUAGE)-alt.aff

#
#	The following dependency can be executed when ispell is unpacked,
#	to unpack the dictionaries.
#
unpack:	$(AFFIXES)

$(UNSQ):
	set +e; \
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
#	The following target is used in the English makefile, and is
#	required to be present in all other language Makefiles as
#	well, even though it doesn't have to do anything in those
#	directories.
#
dictclean:

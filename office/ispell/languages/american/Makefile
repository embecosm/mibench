#
# $Id: Makefile,v 1.4 1995/01/08 23:24:15 geoff Exp $
#
# Copyright 1994, Geoff Kuenning, Granada Hills, CA
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
# This Makefile allows a single installation to build an American
# variant of ispell's English dictionary.  It depends on the
# languages/english directory for most of its input files.
#
# $Log: Makefile,v $
# Revision 1.4  1995/01/08  23:24:15  geoff
# Remember to set SHELLDEBUG when making dictcomponents.
#
# Revision 1.3  1994/08/31  05:58:53  geoff
# Create directories before installing into them, and be sure to set the
# proper modes on manual pages.
#
# Revision 1.2  1994/05/17  06:37:40  geoff
# Explicitly specify the affix files with a path so that initial
# installation will work correctly.
#
# Revision 1.1  1994/04/27  02:46:10  geoff
# Initial revision
#
#

SHELL = /bin/sh
MAKE = make

CONFIG		=	../../config.sh
PATHADDER	=	../..
BUILDHASH	=	../../buildhash

# The following variables should be set by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
# There are four progressively-larger English dictionaries distributed
# with ispell.  These are named english.sml, english.med, english.lrg,
# and english.xlg.  For each of these, you can also build a "plus"
# version (english.sml+, etc.)  which is created by combining the
# distributed version with an "extra" dictionary defined by EXTRADICT,
# usually /usr/dict/words.  The "plus" versions of dictionaries
# require lots of time and temporary file space; make sure you have
# set TMPDIR appropriately.
#
# The dictionaries to be built are listed in the MASTERDICTS variable,
# separated by spaces.  The hash files to be built (and installed) are
# listed in the HASHFILES variable.  Hash files are named as
# "american<xxx>.hash", where <xxx> is the suffix of dictionary (e.g.,
# "med+").  The first-listed hash file will also be installed under
# the name "american.hash".  As a general rule, the dictionaries
# needed to build the HASHFILES must be listed in the MASTERDICTS
# variable.
#
# If you change AFFIXES for english, you should consider also changing
# DEFLANG (in config.X) to match.
#
MASTERDICTS	=	Use_LANGUAGES_from_config.X
HASHFILES	=	Use_LANGUAGES_from_config.X
EXTRADICT	=	Use_LANGUAGES_from_config.X

#
# The following variables may be overridden by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
AFFIXES	=	english.aff
LANGUAGE =	american

#
# DICTSRC specifies where the dictionary sources are found.  You
# should not have to change this.
#
DICTSRC =	../english

#
#	Set this to "-vx" in the make command line if you need to
#	debug the complex shell commands.
#
SHELLDEBUG = +vx

all:  $(CONFIG)
	@set $(SHELLDEBUG); \
	if [ ! -r english.0 ]; \
	then \
	    $(MAKE) SHELLDEBUG=$(SHELLDEBUG) CBUILD= dictcomponents; \
	else \
	    : ; \
	fi
	@set $(SHELLDEBUG); \
	for dict in $(MASTERDICTS); do \
	    if [ ! -r $$dict ]; \
	    then \
		$(MAKE) DBUILD= \
		  'EXTRADICT=$(EXTRADICT)' 'DICTSRC=$(DICTSRC)' \
		  SHELLDEBUG=$(SHELLDEBUG) $$dict; \
	    else \
		: ; \
	    fi; \
	done
	$(MAKE) SHELLDEBUG=$(SHELLDEBUG) $(HASHFILES)

install:	all $(CONFIG)
	@. $(CONFIG); \
	  set -x; \
	  [ -d $$LIBDIR ]  ||  (mkdir $$LIBDIR; chmod 755 $$LIBDIR); \
	  cd $$LIBDIR; rm -f english.aff $(HASHFILES) $(LANGUAGE).hash
	@. $(CONFIG); \
	  set -x; \
	  cp $(DICTSRC)/english.aff $(HASHFILES) $$LIBDIR
	@. $(CONFIG); \
	  set -x; \
	  cd $$LIBDIR; \
	  chmod 644 english.aff $(HASHFILES); \
	  for i in $(HASHFILES); do \
	    ln $$i $(LANGUAGE).hash; \
	    break; \
	  done
	@. $(CONFIG); \
	  set -x; \
	  [ -d $$MAN4DIR ]  ||  (mkdir $$MAN4DIR; chmod 755 $$MAN4DIR); \
	  cd $$MAN4DIR; \
	    rm -f english$$MAN4EXT
	@. $(CONFIG); \
	  set -x; \
	  cp $(DICTSRC)/english.4l $$MAN4DIR/english$$MAN4EXT; \
	  chmod 644 $$MAN4DIR/english$$MAN4EXT

#
#	Dependencies to build extra hash files
#
allhashes:	normhashes plushashes
normhashes:	americansml.hash americanmed.hash
normhashes:	americanlrg.hash americanxlg.hash
plushashes:	americansml+.hash americanmed+.hash
plushashes:	americanlrg+.hash americanxlg+.hash

americansml.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.sml
	rm -f americansml.hash
	$(BUILDHASH) american.sml $(DICTSRC)/$(AFFIXES) americansml.hash

americansml+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.sml+
	rm -f americansml+.hash
	$(BUILDHASH) american.sml+ $(DICTSRC)/$(AFFIXES) americansml+.hash

americanmed.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.med
	rm -f americanmed.hash
	$(BUILDHASH) american.med $(DICTSRC)/$(AFFIXES) americanmed.hash

americanmed+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.med+
	rm -f americanmed+.hash
	$(BUILDHASH) american.med+ $(DICTSRC)/$(AFFIXES) americanmed+.hash

americanlrg.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.lrg
	rm -f americanlrg.hash
	$(BUILDHASH) american.lrg $(DICTSRC)/$(AFFIXES) americanlrg.hash

americanlrg+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.lrg+
	rm -f americanlrg+.hash
	$(BUILDHASH) american.lrg+ $(DICTSRC)/$(AFFIXES) americanlrg+.hash

americanxlg.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.xlg
	rm -f americanxlg.hash
	$(BUILDHASH) american.xlg $(DICTSRC)/$(AFFIXES) americanxlg.hash

americanxlg+.hash:	$(BUILDHASH) $(DICTSRC)/$(AFFIXES) american.xlg+
	rm -f americanxlg+.hash
	$(BUILDHASH) american.xlg+ $(DICTSRC)/$(AFFIXES) americanxlg+.hash

#
#	The eight dictionaries, american.sml through american.xlg+, are
#	built by the following dependencies.  The macros "CBUILD" and
#	"DBUILD" are a sneaky trick used to keep make from trying to
#	run munchlist every time a minor change is made to one of the
#	C programs.  If you define these macros as null, "make all"
#	will automatically build the dictionaries.  If you leave them
#	with their default definitions, "make all" will leave the
#	dictionaries alone, and you will have to type something like
#	"make buildamerican.med+" to build the dictionary using
#	munchlist.
#
#	Why two macros?  So you can delete {eng,amer,altamer,brit}.[012].??
#	to save space, and still type "make DBUILD= all".
#
#	The following dependencies are convenient ways to make some or
#	all of the dictionaries.
#
BUILD	=	build
CBUILD	=	$(BUILD)
DBUILD	=	$(BUILD)

alldicts:	normdicts plusdicts
normdicts:	$(DBUILD)american.sml
normdicts:	$(DBUILD)american.med
normdicts:	$(DBUILD)american.lrg
normdicts:	$(DBUILD)american.xlg
plusdicts:	$(DBUILD)american.sml+
plusdicts:	$(DBUILD)american.med+
plusdicts:	$(DBUILD)american.lrg+
plusdicts:	$(DBUILD)american.xlg+

$(DBUILD)american.sml:	$(DBUILD)english.sml
	rm -f american.sml
	ln english.sml american.sml

$(DBUILD)english.sml:	$(CONFIG)
$(DBUILD)english.sml:	english.0
$(DBUILD)english.sml:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.sml

$(DBUILD)american.sml+:	$(DBUILD)english.sml+
	rm -f american.sml+
	ln english.sml+ american.sml+

$(DBUILD)english.sml+:	$(CONFIG)
$(DBUILD)english.sml+:	english.0
$(DBUILD)english.sml+:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.sml+

$(DBUILD)american.med:	$(DBUILD)english.med
	rm -f american.med
	ln english.med american.med

$(DBUILD)english.med:	$(CONFIG)
$(DBUILD)english.med:	english.0
$(DBUILD)english.med:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.med

$(DBUILD)american.med+:	$(DBUILD)english.med+
	rm -f american.med+
	ln english.med+ american.med+

$(DBUILD)english.med+:	$(CONFIG)
$(DBUILD)english.med+:	english.0
$(DBUILD)english.med+:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.med+

$(DBUILD)american.lrg:	$(DBUILD)english.lrg
	rm -f american.lrg
	ln english.lrg american.lrg

$(DBUILD)english.lrg:	$(CONFIG)
$(DBUILD)english.lrg:	english.0
$(DBUILD)english.lrg:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.lrg

$(DBUILD)american.lrg+:	$(DBUILD)english.lrg+
	rm -f american.lrg+
	ln english.lrg+ american.lrg+

$(DBUILD)english.lrg+:	$(CONFIG)
$(DBUILD)english.lrg+:	english.0
$(DBUILD)english.lrg+:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.lrg+

$(DBUILD)american.xlg:	$(DBUILD)english.xlg
	rm -f american.xlg
	ln english.xlg american.xlg

$(DBUILD)english.xlg:	$(CONFIG)
$(DBUILD)english.xlg:	english.0
$(DBUILD)english.xlg:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.xlg

$(DBUILD)american.xlg+:	$(DBUILD)english.xlg+
	rm -f american.xlg+
	ln english.xlg+ american.xlg+

$(DBUILD)english.xlg+:	$(CONFIG)
$(DBUILD)english.xlg+:	english.0
$(DBUILD)english.xlg+:	american.0
	$(MAKE) -f $(DICTSRC)/Makefile 'DBUILD=$(DBUILD)' VARIANTS=american \
	  'EXTRADICT=$(EXTRADICT)' 'SHELLDEBUG=$(SHELLDEBUG)' \
	  'AFFIXES=$(DICTSRC)/$(AFFIXES)' \
	  english.xlg+

#
#	Dependencies to create the components of the dictionaries,
#	preferably by linking.  We create some components that we'll
#	never use because the master English Makefile requires them.
#
dictcomponents:	$(CBUILD)english.0
dictcomponents:	$(CBUILD)english.1
dictcomponents:	$(CBUILD)english.2
dictcomponents:	$(CBUILD)english.3
dictcomponents: $(CBUILD)american.0
dictcomponents: $(CBUILD)american.1
dictcomponents:	$(CBUILD)american.2
dictcomponents: $(CBUILD)altamer.0
dictcomponents: $(CBUILD)altamer.1
dictcomponents:	$(CBUILD)altamer.2
dictcomponents: $(CBUILD)british.0
dictcomponents: $(CBUILD)british.1
dictcomponents:	$(CBUILD)british.2

$(CBUILD)english.0 $(CBUILD)english.1 $(CBUILD)english.2 $(CBUILD)english.3 \
  $(CBUILD)american.0 $(CBUILD)american.1 $(CBUILD)american.2 \
  $(CBUILD)altamer.0 $(CBUILD)altamer.1 $(CBUILD)altamer.2 \
  $(CBUILD)british.0 $(CBUILD)british.1 $(CBUILD)british.2:
	rm -f english.[0-3] american.[0-2] altamer.[012] british.[012]
	@set $(SHELLDEBUG); \
	  cd $(DICTSRC); \
	  test -r english.0 \
	    ||  $(MAKE) SHELLDEBUG=$(SHELLDEBUG) CBUILD= dictcomponents
	@set $(SHELLDEBUG); \
	for i in english.0 english.1 english.2 english.3 \
	  american.0 american.1 american.2 altamer.0 altamer.1 altamer.2 \
	  british.0 british.1 british.2; do \
	    ln -s $(DICTSRC)/$$i .  ||  ln $(DICTSRC)/$$i . \
	      ||  cp $(DICTSRC)/$$i .; \
	done

#
#	The following dependency can be executed when ispell is unpacked,
#	to unpack the dictionaries.
#
unpack:
	cd $(DICTSRC); $(MAKE) SHELLDEBUG=$(SHELLDEBUG) unpack

clean:
	rm -f core *.hash *.stat *.cnt

#
#	The following target allows you to clean out the leftover raw
#	files gotten from unpacking the kit.  It makes sure that you
#	have the combined files first, so it may take a little while
#	to run.
#
kitclean:
	cd $(DICTSRC); $(MAKE) SHELLDEBUG=$(SHELLDEBUG) kitclean

#
#	The following target allows you to clean out the combined
#	dictionary files.
#
dictclean:
	rm -f english.[0-3] american.[0-3] altamer.[0-3] british.[0-3]
	rm -f *.sml *.sml+ *.med *.med+ *.lrg *.lrg+ *.xlg *.xlg+

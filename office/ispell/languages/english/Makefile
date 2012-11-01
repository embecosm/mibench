#
# $Id: Makefile,v 1.23 1995/08/05 23:19:50 geoff Exp $
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
# This Makefile is an example of how to build dictionaries for a
# complex language with many variants.  It supports both American and
# British English and four dictionary source files, and will build up
# to 8 dictionares from those 4 files by combining them with an
# optional installation-specific file.
#
# If you are building a new Makefile for your own language, this is
# probably not the right place to start.  Instead, you should select
# the "deutsch" Makefile, which is moderately complex, or one of the
# simple Makefiles for another language distributed with ispell.
#
# $Log: Makefile,v $
# Revision 1.23  1995/08/05  23:19:50  geoff
# Add tests to detect and suppress accidentally-generated zero-length
# dictionaries.
#
# Revision 1.22  1994/08/31  05:58:43  geoff
# Create directories before installing into them, and be sure to set the
# proper modes on manual pages.
#
# Revision 1.21  1994/05/25  04:29:30  geoff
# Get rid of all references to DEFHASH and defhash.
#
# Revision 1.20  1994/05/24  04:47:33  geoff
# Work around an RS-6000 shell deficiency in environment setting
#
# Revision 1.19  1994/05/17  06:37:36  geoff
# Fix the remove-on-abort code for the derived dictionaries to be more
# robust.
#
# Revision 1.18  1994/02/23  04:52:30  geoff
# Remove dictionaries being built if make is aborted early.
#
# Revision 1.17  1994/02/22  06:09:06  geoff
# Add SHELLDEBUG.
#
# Revision 1.16  1994/02/13  23:55:20  geoff
# Get rid of the temporary line that zapped "english..4" for people who
# had 3.1.00.
#
# Revision 1.15  1994/02/07  08:01:57  geoff
# Allow multiple variants and extradicts
#
# Revision 1.14  1994/02/07  07:08:10  geoff
# Install english.4 with the right name, removing the wrong one if it exists
#
# Revision 1.13  1994/02/07  06:18:18  geoff
# Add dummy else clauses to shell tests to handle Ultrix problems.  Add an
# eval to all sort commands to make sure that MAKE_SORTTMP is handled
# correctly on all systems.
#
# Revision 1.12  1994/01/25  08:50:34  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#

SHELL = /bin/sh
MAKE = make

CONFIG		=	../../config.sh
PATHADDER	=	../..
BUILDHASH	=	../../buildhash
UNSQ		=	../../unsq

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
# listed in the HASHFILES variable.  Hash files are named by taking
# the suffix of the dictionary (e.g., "med+"), and adding ".hash".  As
# a general rule, the dictionaries needed to build the HASHFILES must
# be listed in the MASTERDICTS variable.
#
# British/American variants are supported by the VARIANTS variable.
# You should set VARIANTS to one of "american", "british", or
# "altamer".  The latter is a file of alternate American spellings,
# often British-derived.  I recommend against its use.  I also
# recommend against selecting more than one variant, because it tends
# to cause inconsistent or incorrect spellings to be hidden, although
# there is nothing to prevent such an unwise decision.
#
# If you change AFFIXES for english, you should consider also changing
# DEFLANG (in config.X) to match.
#
MASTERDICTS	=	Use_LANGUAGES_from_config.X
HASHFILES	=	Use_LANGUAGES_from_config.X
VARIANTS	=	Use_LANGUAGES_from_config.X
EXTRADICT	=	Use_LANGUAGES_from_config.X

#
# The following variables may be overridden by the superior Makefile,
# based on the LANGUAGES variable in config.X.
#
AFFIXES	=	english.aff

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
		$(MAKE) DBUILD= 'VARIANTS=$(VARIANTS)' \
		  'EXTRADICT=$(EXTRADICT)' \
		  SHELLDEBUG=$(SHELLDEBUG) $$dict; \
	    else \
		: ; \
	    fi; \
	done
	$(MAKE) SHELLDEBUG=$(SHELLDEBUG) $(HASHFILES)

install: all $(CONFIG)
	@. $(CONFIG); \
	  set -x; \
	  [ -d $$LIBDIR ]  ||  (mkdir $$LIBDIR; chmod 755 $$LIBDIR); \
	  cd $$LIBDIR; rm -f english.aff $(HASHFILES)
	@. $(CONFIG); \
	  set -x; \
	  cp english.aff $(HASHFILES) $$LIBDIR
	@. $(CONFIG); \
	  set -x; \
	  cd $$LIBDIR; \
	  chmod 644 english.aff $(HASHFILES)
	@. $(CONFIG); \
	  set -x; \
	  cd $$MAN4DIR; \
	  [ -d $$MAN4DIR ]  ||  (mkdir $$MAN4DIR; chmod 755 $$MAN4DIR); \
	    rm -f english$$MAN4EXT
	@. $(CONFIG); \
	  set -x; \
	  cp english.4l $$MAN4DIR/english$$MAN4EXT; \
	  chmod 644 $$MAN4DIR/english$$MAN4EXT

#
#	Dependencies to build extra hash files
#
allhashes:	normhashes plushashes
normhashes:	sml.hash med.hash lrg.hash xlg.hash
plushashes:	sml+.hash med+.hash lrg+.hash xlg+.hash

#
# Note that we don't use $(MAKE) in the following dependencies.  There
# is a good reason for this -- if we did, "make -n" would still run
# buildhash.
#
sml.hash:	$(CONFIG) $(BUILDHASH)
sml.hash:	$(AFFIXES)
sml.hash:	english.sml
	rm -f sml.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.sml $(AFFIXES) $@

sml+.hash:	$(CONFIG) $(BUILDHASH)
sml+.hash:	$(AFFIXES)
sml+.hash:	english.sml+
	rm -f sml+.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.sml+ $(AFFIXES) $@

med.hash:	$(BUILDHASH) $(CONFIG)
med.hash:	$(AFFIXES)
med.hash:	english.med
	rm -f med.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.med $(AFFIXES) $@

med+.hash:	$(BUILDHASH) $(CONFIG)
med+.hash:	$(AFFIXES)
med+.hash:	english.med+
	rm -f med+.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.med+ $(AFFIXES) $@

lrg.hash:	$(BUILDHASH) $(CONFIG)
lrg.hash:	$(AFFIXES)
lrg.hash:	english.lrg
	rm -f lrg.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.lrg $(AFFIXES) $@

lrg+.hash:	$(BUILDHASH) $(CONFIG)
lrg+.hash:	$(AFFIXES)
lrg+.hash:	english.lrg+
	rm -f lrg+.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.lrg+ $(AFFIXES) $@

xlg.hash:	$(BUILDHASH) $(CONFIG)
xlg.hash:	$(AFFIXES)
xlg.hash:	english.xlg
	rm -f xlg.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.xlg $(AFFIXES) $@

xlg+.hash:	$(BUILDHASH) $(CONFIG)
xlg+.hash:	$(AFFIXES)
xlg+.hash:	english.xlg+
	rm -f xlg+.hash
	@set +e; \
	. $(CONFIG); \
	set -ex; \
	$(BUILDHASH) english.xlg+ $(AFFIXES) $@

#
#	The eight dictionaries, english.sml through english.xlg+, are
#	built by the following dependencies.  The macros "CBUILD" and
#	"DBUILD" are a sneaky trick used to keep make from trying to
#	run munchlist every time a minor change is made to one of the
#	C programs.  If you define these macros as null, "make all"
#	will automatically build the dictionaries.  If you leave them
#	with their default definitions, "make all" will leave the
#	dictionaries alone, and you will have to type something like
#	"make buildenglish.med+" to build the dictionary using
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
normdicts:	$(DBUILD)english.sml
normdicts:	$(DBUILD)english.med
normdicts:	$(DBUILD)english.lrg
normdicts:	$(DBUILD)english.xlg
plusdicts:	$(DBUILD)english.sml+
plusdicts:	$(DBUILD)english.med+
plusdicts:	$(DBUILD)english.lrg+
plusdicts:	$(DBUILD)english.xlg+

dictcomponents:	$(CBUILD)english.0
dictcomponents:	$(CBUILD)english.1
dictcomponents:	$(CBUILD)english.2
dictcomponents:	$(CBUILD)english.3
dictcomponents: $(CBUILD)american.0
dictcomponents: $(CBUILD)american.1
dictcomponents:	$(CBUILD)american.2
dictcomponents: $(CBUILD)altamer.0
dictcomponents:	$(CBUILD)altamer.1
dictcomponents:	$(CBUILD)altamer.2
dictcomponents: $(CBUILD)british.0
dictcomponents:	$(CBUILD)british.1
dictcomponents:	$(CBUILD)british.2

$(CBUILD)english.0:	$(UNSQ) eng.0.??
	cat eng.0.?? | $(UNSQ) > english.0

$(CBUILD)english.1:	$(UNSQ) eng.1.??
	cat eng.1.?? | $(UNSQ) > english.1

$(CBUILD)english.2:	$(UNSQ) eng.2.??
	cat eng.2.?? | $(UNSQ) > english.2

$(CBUILD)english.3:	$(UNSQ) eng.3.??
	cat eng.3.?? | $(UNSQ) > english.3

$(CBUILD)american.0:	$(UNSQ) amer.0.??
	cat amer.0.?? | $(UNSQ) > american.0

$(CBUILD)american.1:	$(UNSQ) amer.1.??
	cat amer.1.?? | $(UNSQ) > american.1

$(CBUILD)american.2:	$(UNSQ) amer.2.??
	cat amer.2.?? | $(UNSQ) > american.2

$(CBUILD)altamer.0:	$(UNSQ) altamer.0.??
	cat altamer.0.?? | $(UNSQ) \
	  > altamer.0

$(CBUILD)altamer.1:	$(UNSQ) altamer.1.??
	cat altamer.1.?? | $(UNSQ) \
	  > altamer.1

$(CBUILD)altamer.2:	$(UNSQ) altamer.2.??
	cat altamer.2.?? | $(UNSQ) \
	  > altamer.2

$(CBUILD)british.0:	$(UNSQ) brit.0.??
	cat brit.0.?? | $(UNSQ) > british.0

$(CBUILD)british.1:	$(UNSQ) brit.1.??
	cat brit.1.?? | $(UNSQ) > british.1

$(CBUILD)british.2:	$(UNSQ) brit.2.??
	cat brit.2.?? | $(UNSQ) > british.2

$(DBUILD)english.sml:	$(CONFIG)
$(DBUILD)english.sml:	english.0
$(DBUILD)english.sml:	american.0
$(DBUILD)english.sml:	altamer.0
$(DBUILD)english.sml:	british.0
	@. $(CONFIG); \
	    set $(SHELLDEBUG); \
	    dicts=""; \
	    for i in english $(VARIANTS); do \
		dicts="$$dicts $$i.0"; \
	    done; \
	    trap "rm -f english.sml" 1 2 15; \
	    set -x; \
	    eval sort -u -t/ +0f -1 +0 $$MAKE_SORTTMP -o english.sml $$dicts

$(DBUILD)english.sml+:	$(CONFIG) $(EXTRADICT)
$(DBUILD)english.sml+:	english.0
$(DBUILD)english.sml+:	american.0
$(DBUILD)english.sml+:	altamer.0
$(DBUILD)english.sml+:	british.0
	@dicts="$(EXTRADICT)"; \
	set $(SHELLDEBUG); \
	for i in english $(VARIANTS); do \
	    dicts="$$dicts $$i.0"; \
	done; \
	trap "rm -f english.sml+" 1 2 15; \
	set -x; \
	PATH=$(PATHADDER):$$PATH; \
	export PATH; \
	munchlist -v -l $(AFFIXES) $$dicts \
	  > english.sml+ \
	    ||  rm -f english.sml+
	test -s english.sml+ \
	  ||  (echo 'error: zero-length dictionary generated'; \
	    rm -f english.sml+; exit 1)

$(DBUILD)english.med:	$(CONFIG)
$(DBUILD)english.med:	english.0
$(DBUILD)english.med:	american.0
$(DBUILD)english.med:	altamer.0
$(DBUILD)english.med:	british.0
$(DBUILD)english.med:	english.1
$(DBUILD)english.med:	american.1
$(DBUILD)english.med:	altamer.1
$(DBUILD)english.med:	british.1
	@. $(CONFIG); \
	    set $(SHELLDEBUG); \
	    dicts=""; \
	    for i in english $(VARIANTS); do \
		dicts="$$dicts $$i.[01]"; \
	    done; \
	    trap "rm -f english.med" 1 2 15; \
	    set -x; \
	    eval sort -u -t/ +0f -1 +0 $$MAKE_SORTTMP -o english.med $$dicts

$(DBUILD)english.med+:	$(CONFIG) $(EXTRADICT)
$(DBUILD)english.med+:	english.0
$(DBUILD)english.med+:	american.0
$(DBUILD)english.med+:	altamer.0
$(DBUILD)english.med+:	british.0
$(DBUILD)english.med+:	english.1
$(DBUILD)english.med+:	american.1
$(DBUILD)english.med+:	altamer.1
$(DBUILD)english.med+:	british.1
	@dicts="$(EXTRADICT)"; \
	set $(SHELLDEBUG); \
	for i in english $(VARIANTS); do \
	    dicts="$$dicts $$i.[01]"; \
	done; \
	trap "rm -f english.med+" 1 2 15; \
	set -x; \
	PATH=$(PATHADDER):$$PATH; \
	export PATH; \
	munchlist -v -l $(AFFIXES) $$dicts \
	  > english.med+ \
	    ||  rm -f english.med+
	test -s english.med+ \
	  ||  (echo 'error: zero-length dictionary generated'; \
	    rm -f english.med+; exit 1)

$(DBUILD)english.lrg:	$(CONFIG)
$(DBUILD)english.lrg:	english.0
$(DBUILD)english.lrg:	american.0
$(DBUILD)english.lrg:	altamer.0
$(DBUILD)english.lrg:	british.0
$(DBUILD)english.lrg:	english.1
$(DBUILD)english.lrg:	american.1
$(DBUILD)english.lrg:	altamer.1
$(DBUILD)english.lrg:	british.1
$(DBUILD)english.lrg:	english.2
$(DBUILD)english.lrg:	american.2
$(DBUILD)english.lrg:	altamer.2
$(DBUILD)english.lrg:	british.2
	@. $(CONFIG); \
	    set $(SHELLDEBUG); \
	    dicts=""; \
	    for i in english $(VARIANTS); do \
		dicts="$$dicts $$i.[012]"; \
	    done; \
	    trap "rm -f english.lrg" 1 2 15; \
	    set -x; \
	    eval sort -u -t/ +0f -1 +0 $$MAKE_SORTTMP -o english.lrg $$dicts

$(DBUILD)english.lrg+:	$(CONFIG) $(EXTRADICT)
$(DBUILD)english.lrg+:	english.0
$(DBUILD)english.lrg+:	american.0
$(DBUILD)english.lrg+:	altamer.0
$(DBUILD)english.lrg+:	british.0
$(DBUILD)english.lrg+:	english.1
$(DBUILD)english.lrg+:	american.1
$(DBUILD)english.lrg+:	altamer.1
$(DBUILD)english.lrg+:	british.1
$(DBUILD)english.lrg+:	english.2
$(DBUILD)english.lrg+:	american.2
$(DBUILD)english.lrg+:	altamer.2
$(DBUILD)english.lrg+:	british.2
	@dicts="$(EXTRADICT)"; \
	set $(SHELLDEBUG); \
	for i in english $(VARIANTS); do \
	    dicts="$$dicts $$i.[012]"; \
	done; \
	trap "rm -f english.lrg+" 1 2 15; \
	set -x; \
	PATH=$(PATHADDER):$$PATH; \
	export PATH; \
	munchlist -v -l $(AFFIXES) $$dicts \
	  > english.lrg+ \
	    ||  rm -f english.lrg+
	test -s english.lrg+ \
	  ||  (echo 'error: zero-length dictionary generated'; \
	    rm -f english.lrg+; exit 1)

$(DBUILD)english.xlg:	$(CONFIG)
$(DBUILD)english.xlg:	english.0
$(DBUILD)english.xlg:	american.0
$(DBUILD)english.xlg:	altamer.0
$(DBUILD)english.xlg:	british.0
$(DBUILD)english.xlg:	english.1
$(DBUILD)english.xlg:	american.1
$(DBUILD)english.xlg:	altamer.1
$(DBUILD)english.xlg:	british.1
$(DBUILD)english.xlg:	english.2
$(DBUILD)english.xlg:	american.2
$(DBUILD)english.xlg:	altamer.2
$(DBUILD)english.xlg:	british.2
$(DBUILD)english.xlg:	english.3
	@. $(CONFIG); \
	    set $(SHELLDEBUG); \
	    dicts=""; \
	    for i in english $(VARIANTS); do \
		dicts="$$dicts $$i.[0123]"; \
	    done; \
	    trap "rm -f english.xlg" 1 2 15; \
	    set -x; \
	    eval sort -u -t/ +0f -1 +0 $$MAKE_SORTTMP -o english.xlg $$dicts

$(DBUILD)english.xlg+:	$(CONFIG) $(EXTRADICT)
$(DBUILD)english.xlg+:	english.0
$(DBUILD)english.xlg+:	american.0
$(DBUILD)english.xlg+:	altamer.0
$(DBUILD)english.xlg+:	british.0
$(DBUILD)english.xlg+:	english.1
$(DBUILD)english.xlg+:	american.1
$(DBUILD)english.xlg+:	altamer.1
$(DBUILD)english.xlg+:	british.1
$(DBUILD)english.xlg+:	english.2
$(DBUILD)english.xlg+:	american.2
$(DBUILD)english.xlg+:	altamer.2
$(DBUILD)english.xlg+:	british.2
$(DBUILD)english.xlg+:	english.3
	@dicts="$(EXTRADICT)"; \
	set $(SHELLDEBUG); \
	for i in english $(VARIANTS); do \
	    dicts="$$dicts $$i.[0123]"; \
	done; \
	trap "rm -f english.xlg+" 1 2 15; \
	set -x; \
	PATH=$(PATHADDER):$$PATH; \
	export PATH; \
	munchlist -v -l $(AFFIXES) $$dicts \
	  > english.xlg+ \
	    ||  rm -f english.xlg+
	test -s english.xlg+ \
	  ||  (echo 'error: zero-length dictionary generated'; \
	    rm -f english.xlg+; exit 1)

#
#	The following dependency can be executed when ispell is unpacked,
#	to unpack the dictionaries.
#
unpack:	$(UNSQ)
	$(MAKE) CBUILD= SHELLDEBUG=$(SHELLDEBUG) dictcomponents
	$(MAKE) SHELLDEBUG=$(SHELLDEBUG) kitclean

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
#	The following target allows you to clean out the leftover raw
#	files gotten from unpacking the kit.  It makes sure that you
#	have the combined files first, so it may take a little while
#	to run.
#
kitclean:	$(UNSQ)
	$(MAKE) CBUILD= SHELLDEBUG=$(SHELLDEBUG) dictcomponents
	rm -f eng.[0123].?? amer.[012].?? altamer.[012].?? brit.[012].??

#
#	The following target allows you to clean out the combined
#	dictionary files.
#
dictclean:
	rm -f english.sml english.sml+ english.med english.med+
	rm -f english.lrg english.lrg+ english.xlg english.xlg+

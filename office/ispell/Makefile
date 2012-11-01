#
# $Id: Makefile,v 1.102 1995/10/11 04:30:25 geoff Exp $
#
# Copyright 1992, 1993, Geoff Kuenning, Granada Hills, CA
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
# You will have to create a local.h file before building; look over
# config.X to learn what things you may need to define, or use one of
# the sample local.h files shipped.
#
# the argument syntax for buildhash to make alternate dictionary files
# is simply:
#
#   buildhash <dictfile> <affix-file> <outfile>

# $Log: Makefile,v $
# Revision 1.102  1995/10/11  04:30:25  geoff
# Allow DEFHASH and MASTERHASH to be the same.
#
# Revision 1.101  1995/08/05  23:19:33  geoff
# Add the showversion target to ease the job of debugging people's make
# output.  Make sure SHELLDEBUG is set for the entire process of making
# config.sh.
#
# Revision 1.100  1995/03/06  02:42:39  geoff
# Run iwhich explicitly from the shell, so that we don't have to worry
# about PATH problems and whether it's executable or not.  Also fix a
# place where -nw was not removed from the batch emacs switch list.
#
# Revision 1.99  1995/01/08  23:23:23  geoff
# Get rid of an obsolete etags flag.  Add some new variables to config.sh.
#
# Revision 1.98  1994/12/27  23:08:43  geoff
# Use the new "iwhich" script to decide whether to install emacs-related
# stuff.  Make correct.o depend on version.h so that "ispell -a" always
# reports the correct version.
#
# Revision 1.97  1994/11/21  07:02:51  geoff
# Specify default values for the BUILD macros, so that some systems
# don't accidentally override them and make ispell think that the
# dictionaries are missing.
#
# Revision 1.96  1994/10/25  05:45:54  geoff
# Make the installation command configurable.
#
# Revision 1.95  1994/10/18  04:03:17  geoff
# Get rid of DICTVARIANTS, which is obsolete.  Compile term.o first, so
# that errors in it (which are common) will show up first.  Improve the
# rules for generation of msgs.h.
#
# Revision 1.94  1994/09/16  05:06:55  geoff
# Split installation up into basic and dictionary-building tools, so
# that we can have a partial-install target.
#
# Revision 1.93  1994/09/16  04:51:28  geoff
# Handle installations that have ELISPDIR but not TEXINFODIR
#
# Revision 1.92  1994/09/16  02:45:52  geoff
# Don't strip non-binaries.  Fix an accidentally-doubled backslash.
#
# Revision 1.91  1994/08/31  05:58:27  geoff
# Strip binaries before installing them.  Create directories before
# installing into them.  Make sure manual pages are installed with the
# correct protection modes.
#
# Revision 1.90  1994/05/25  04:29:16  geoff
# Don't remove english.4 after the English makefile has carefully
# installed it.
#
# Revision 1.89  1994/05/24  05:31:22  geoff
# Return to the old sed-based method of parsing LANGUAGES, so that things
# will work on broken systems like BSDI.
#
# Revision 1.88  1994/05/24  04:54:30  geoff
# Fix the emacs installation to use emacs batch mode properly, so that a
# terminal isn't required.
#
# Revision 1.87  1994/03/21  01:55:17  geoff
# If a hard link can't be made to msgs.h, copy it instead
#
# Revision 1.86  1994/02/22  06:09:03  geoff
# Add SHELLDEBUG.  Change the language-subdirs target to use the shell
# IFS variable to parse things, simplifying things and improving
# efficiency (thanks to Hagen Ross for the idea and implemenation).
#
# Revision 1.85  1994/02/13  23:25:31  geoff
# Fix multiple-language processing to not pass subsequent specifications to
# the first Makefile.  Also fix the language shell loop to be more flexible.
#
# Revision 1.84  1994/02/07  08:10:40  geoff
# When processing the LANGUAGES configuration variable (from local.h),
# use sed instead of expr to process it.  This gets around versions of
# expr that have 127-character limitations (though it probably still
# limits us to 512 characters with some versions of sed, so further work
# may be needed here).
#
# Revision 1.83  1994/02/07  06:31:20  geoff
# Clarify how to change variables in local.h
#
# Revision 1.82  1994/02/07  06:29:31  geoff
# Add a dummy else clause to shell if-test for Ultrix
#
# Revision 1.81  1994/02/07  05:35:34  geoff
# Make realclean run dictclean
#
# Revision 1.80  1994/01/26  07:44:43  geoff
# Make yacc configurable through local.h.
#
# Revision 1.79  1994/01/25  07:11:11  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#

#
# !!!DO NOT EDIT HERE!!!
#
# Unlike previous versions of ispell, there should be no need to edit
# your Makefile.  Instead, #define the corresponding variables in your
# local.h file; the Makefile will automatically pick them up.  The
# only reason you should need to edit the Makefile might to be to add
# non-English dictionary support.
#
# For example, if you want to set CFLAGS to "-g -Wall", don't put it
# here.  Put:
#
#	#define CFLAGS	"-g -Wall"
#
# in local.h.  Otherwise, it won't have any effect.
#
EXTRADICT = Use_config.sh

SHELL = /bin/sh
MAKE = make

#
#	Set this to "-vx" in the make command line if you need to
#	debug the complex shell commands.
#
SHELLDEBUG = +vx

all:	unpacked
all:	config.sh
all:	programs showversion ispell.1 ispell.info
all:	all-languages

programs: buildhash findaffix tryaffix ispell
programs: icombine ijoin munchlist
programs: subset sq unsq zapdups

showversion:
	./ispell -v | head -1

.c.o:
	@. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -c $<

.y.o:
	@. ./config.sh; \
	  set -x; \
	  $$YACC $<; \
	  $$CC $$CFLAGS -c y.tab.c; \
	  mv y.tab.o $@; \
	  rm -f y.tab.c

all-languages:	munchable
	$(MAKE) LANGUAGE_TARGET=all SHELLDEBUG=$(SHELLDEBUG) language-subdirs

install: config.sh all install-basic install-dictbuild install-languages

partial-install: config.sh all install-basic install-languages

install-basic:
	@. ./config.sh; \
	  set -x; \
	  [ -d $$BINDIR ]  ||  (mkdir $$BINDIR; chmod 755 $$BINDIR); \
	  cd $$BINDIR; \
	  rm -f ispell
	@. ./config.sh; \
	  set -x; \
	  $$INSTALL ispell $$BINDIR
	@. ./config.sh; \
	  set -x; \
	  cd $$BINDIR; \
	  strip ispell; \
	  chmod 755 ispell
	@. ./config.sh; \
	  set -x; \
	  [ -d $$MAN1DIR ]  ||  (mkdir $$MAN1DIR; chmod 755 $$MAN1DIR); \
	  [ -d $$MAN4DIR ]  ||  (mkdir $$MAN4DIR; chmod 755 $$MAN4DIR); \
	  cd $$MAN1DIR; \
	    rm -f ispell$$MAN1EXT; \
	  cd $$MAN4DIR; \
	    rm -f ispell$$MAN4EXT
	@. ./config.sh; \
	  set -x; \
	  $$INSTALL ispell.1 $$MAN1DIR/ispell$$MAN1EXT; \
	  $$INSTALL ispell.4 $$MAN4DIR/ispell$$MAN4EXT
	@. ./config.sh; \
	  set -x; \
	  cd $$MAN1DIR; \
	  chmod 644 ispell$$MAN1EXT; \
	  cd $$MAN4DIR; \
	  chmod 644 ispell$$MAN4EXT
	@set +e; \
	  . ./config.sh; \
	  set $(SHELLDEBUG); \
	  if [ -d $$TEXINFODIR -a -r ispell.info ]; then \
	    set -ex; \
	    rm -f $$TEXINFODIR/ispell; \
	    $$INSTALL ispell.info $$TEXINFODIR/ispell; \
	    chmod 644 $$TEXINFODIR/ispell; \
	  else \
	    : ; \
	  fi
	@set +e; \
	  . ./config.sh; \
	  set $(SHELLDEBUG); \
	  if [ -d $$ELISPDIR ]; then \
	    set -ex; \
	    rm -f $$ELISPDIR/ispell.el; \
	    $$INSTALL ispell.el $$ELISPDIR; \
	    if sh iwhich $$EMACS >/dev/null; then \
		echo '(byte-compile-file "'"$$ELISPDIR/ispell.el"'")' \
		  > /tmp/emi$$$$; \
		$$EMACS -batch -l /tmp/emi$$$$; \
		rm -f /tmp/emi$$$$; \
		chmod 644 $$ELISPDIR/ispell.el $$ELISPDIR/ispell.elc; \
	    else \
		chmod 644 $$ELISPDIR/ispell.el; \
	    fi; \
	  else \
	    : ; \
	  fi

install-dictbuild:
	@. ./config.sh; \
	  set -x; \
	  [ -d $$BINDIR ]  ||  (mkdir $$BINDIR; chmod 755 $$BINDIR); \
	  cd $$BINDIR; \
	  rm -f buildhash icombine ijoin \
	    munchlist findaffix tryaffix sq unsq; \
	  rm -f $$LIBDIR/icombine
	@. ./config.sh; \
	  set -x; \
	  $$INSTALL buildhash icombine ijoin munchlist findaffix tryaffix \
	      sq unsq \
	    $$BINDIR
	@. ./config.sh; \
	  set -x; \
	  cd $$BINDIR; \
	  strip buildhash icombine ijoin sq unsq; \
	  chmod 755 buildhash icombine ijoin \
	    munchlist findaffix tryaffix sq unsq
	@. ./config.sh; \
	  set -x; \
	  [ -d $$MAN1DIR ]  ||  (mkdir $$MAN1DIR; chmod 755 $$MAN1DIR); \
	  [ -d $$MAN4DIR ]  ||  (mkdir $$MAN4DIR; chmod 755 $$MAN4DIR); \
	  cd $$MAN1DIR; \
	    rm -f sq$$MAN1EXT
	@. ./config.sh; \
	  set -x; \
	  $$INSTALL sq.1 $$MAN1DIR/sq$$MAN1EXT; \
	  for m in buildhash munchlist findaffix tryaffix; do \
	    echo ".so `basename $$MAN1DIR`/ispell$$MAN1EXT" \
	      > $$MAN1DIR/$$m$$MAN1EXT; \
	  done; \
	  echo ".so `basename $$MAN1DIR`/sq$$MAN1EXT" \
	    > $$MAN1DIR/unsq$$MAN1EXT 
	@. ./config.sh; \
	  set -x; \
	  cd $$MAN1DIR; \
	  chmod 644 sq$$MAN1EXT buildhash$$MAN1EXT \
	    munchlist$$MAN1EXT findaffix$$MAN1EXT tryaffix$$MAN1EXT

install-languages:
	$(MAKE) LANGUAGE_TARGET=install SHELLDEBUG=$(SHELLDEBUG) \
	  language-subdirs
	. ./config.sh; \
	  [ -d $$LIBDIR ]  ||  (mkdir $$LIBDIR; chmod 755 $$LIBDIR); \
	  set -x; \
	  cd $$LIBDIR; \
	  if [ $$MASTERHASH != $DEFHASH ]; then \
	    rm -f $$DEFHASH; \
	    ln $$MASTERHASH $$DEFHASH; \
	  fi

ispell.info:	config.sh ispell.texinfo
	set +e; \
	  . ./config.sh; \
	  set $(SHELLDEBUG); \
	  if [ -d $$TEXINFODIR ]; then \
	    if sh iwhich makeinfo >/dev/null \
	      &&  makeinfo ispell.texinfo 2>/dev/null; then \
		:; \
	    elif sh iwhich $$EMACS >/dev/null; then \
		set -e; \
		$$EMACS -batch dummy -i ispell.texinfo \
		  -f texinfo-format-buffer -f save-buffer; \
	    fi; \
	    rm -f ispell.info~; \
	  else \
	    : ; \
	  fi

munchable:	findaffix tryaffix munchlist buildhash ispell icombine
munchable:	ijoin
munchable:	sq unsq

#
#	The following auxiliary dependency is used to make targets in
#	the language directories.  Do you find it intimidating?  No
#	surprise;  remember that this is by the guy who wrote munchlist.
#
LANGUAGE_TARGET	=   Do_not_try_to_make_this_target_yourself
BUILD	=	build
CBUILD	=	build
DBUILD	=	build

language-subdirs:	config.sh
	@. ./config.sh; \
	    set $(SHELLDEBUG); \
	    set +e; \
	    while [ "X$$LANGUAGES" != X ]; do \
		( \
		descriptor=`echo "$$LANGUAGES" \
		  | sed 's/[^{]*{\([^}]*\)}.*/\1/'`; \
		dir=`echo "$$descriptor" | sed 's/\([^,]*\).*/\1/'`; \
		descriptor=`echo "$$descriptor" \
		  | sed 's/[^,]*,*\(.*\).*/\1/'`; \
		makeargs=''; \
		while [ "X$$descriptor" != X ]; \
		do \
		    nextvar=`echo "$$descriptor" | sed 's/\([^,]*\).*/\1/'`; \
		    makeargs="$$makeargs '$$nextvar'"; \
		    descriptor=`echo "$$descriptor" \
		      | sed 's/[^,]*,*\(.*\).*/\1/'`; \
		done; \
		set -x; \
		cd languages/$$dir; \
		eval $(MAKE) BUILD=$(BUILD) DBUILD=$(DBUILD) CBUILD=$(CBUILD) \
		  SHELLDEBUG=$(SHELLDEBUG) "$$makeargs" $(LANGUAGE_TARGET) \
		    ||  exit 1; \
		)  ||  exit 1; \
		LANGUAGES=`echo "$$LANGUAGES" \
		  | sed 's/[^{]*{[^}]*}[^{]*\(.*\)$$/\1/'`; \
		case "$$LANGUAGES" in \
		    ''|*{*}*) \
			;; \
		    *) \
			echo "Bad language specification: '$$LANGUAGES'" \
			  1>&2; \
			exit 2 \
			;; \
		esac; \
	    done; \
	    exit 0
	    
buildhash: config.sh buildhash.o hash.o makedent.o parse.o
	@. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -o buildhash buildhash.o hash.o makedent.o parse.o \
	    $$LIBES

icombine: config.sh icombine.o makedent.o parse.o
	. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -o icombine icombine.o makedent.o parse.o \
	    $$LIBES

ijoin: config.sh ijoin.o fields.o
	@. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -o ijoin ijoin.o fields.o $$LIBES

EDITFILE	=	notthere
OUTFILE		=	/dev/null

config.sh:  config.X local.h
	set $(SHELLDEBUG); \
	for var in BINDIR CC CFLAGS COUNTSUFFIX DEFDICT DEFHASH DEFLANG \
	  ELISPDIR EMACS HASHSUFFIX INSTALL \
	  LANGUAGES LIBDIR LIBES LINT LINTFLAGS \
	  MAKE_SORTTMP MAN1DIR MAN1EXT MAN4DIR MAN4EXT MASTERHASH \
	  MSGLANG REGLIB STATSUFFIX \
	  TERMLIB TEXINFODIR YACC \
	  ; do \
	    cat config.X local.h \
	      | sed -n -e "s/^#define[ 	]*$$var[ 	]*"'"'"/$$var=/p" \
	      | sed -e 's/".*$$/'"'/" -e "s/=/='/" \
	      | tail -1; \
	  done > config.sh; \
	echo 'case "$$MAKE_SORTTMP" in "") \
	  SORTTMP="-e /!!SORTTMP!!/s/=.*$$/=/";; *) SORTTMP=;; esac' \
	  >> config.sh

doedit:
	. ./config.sh; \
	  sed -e "s@!!LIBDIR!!@$$LIBDIR@" -e "s@!!DEFDICT!!@$$DEFDICT@" \
	    -e "s@!!DEFHASH!!@$$DEFHASH@" -e "s@!!DEFLANG!!@$$DEFLANG@" \
	    -e "s@!!COUNTSUFFIX!!@$$COUNTSUFFIX@g" \
	    -e "s@!!HASHSUFFIX!!@$$HASHSUFFIX@g" \
	    -e "s@!!STATSUFFIX!!@$$STATSUFFIX@g" \
	    $$SORTTMP < $(EDITFILE) > $(OUTFILE)

findaffix:	findaffix.X config.sh
	@$(MAKE) EDITFILE=findaffix.X OUTFILE=findaffix doedit
	chmod +x findaffix

ispell.1:	ispell.1X config.sh
	@$(MAKE) EDITFILE=ispell.1X OUTFILE=ispell.1 SHELLDEBUG=$(SHELLDEBUG) \
	  doedit

munchlist:	munchlist.X config.sh
	@$(MAKE) EDITFILE=munchlist.X OUTFILE=munchlist \
	  SHELLDEBUG=$(SHELLDEBUG) doedit
	chmod +x munchlist

subset:	subset.X config.sh
	@$(MAKE) EDITFILE=subset.X OUTFILE=subset SHELLDEBUG=$(SHELLDEBUG) \
	  doedit
	chmod +x subset

tryaffix:	tryaffix.X config.sh
	@$(MAKE) EDITFILE=tryaffix.X OUTFILE=tryaffix \
	  SHELLDEBUG=$(SHELLDEBUG) doedit
	chmod +x tryaffix

zapdups:	zapdups.X config.sh
	@$(MAKE) EDITFILE=zapdups.X OUTFILE=zapdups SHELLDEBUG=$(SHELLDEBUG) \
	  doedit
	chmod +x zapdups

OBJS	=	term.o ispell.o correct.o defmt.o dump.o good.o lookup.o \
		hash.o makedent.o tgood.o tree.o xgets.o

ispell: config.sh $(OBJS)
	@. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -o ispell $(OBJS) $$TERMLIB $$REGLIB $$LIBES

#	Since some makes don't have appropriate built-in rules, here are
#	dependencies for sq and unsq.  Sigh.
sq:	config.sh msgs.h sq.c
	@. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -o sq sq.c

unsq:	config.sh msgs.h unsq.c
	@. ./config.sh; \
	  set -x; \
	  $$CC $$CFLAGS -o unsq unsq.c

$(OBJS) buildhash.o icombine.o hash.o parse.o: config.h ispell.h local.h
$(OBJS) buildhash.o icombine.o hash.o parse.o: proto.h msgs.h
ijoin.o: config.h ispell.h local.h
ijoin.o: proto.h fields.h
buildhash.o correct.o ispell.o: version.h

config.h:	config.X local.h
	cp config.X config.h
	chmod u+w config.h
	echo '' >> config.h
	echo '/* AUTOMATICALLY-GENERATED SYMBOLS */' >> config.h
	cat local.h config.X \
	  | egrep '^#define[ 	]*SIGNAL_TYPE' \
	  | sed -e 's/TYPE[ 	]*/TYPE_STRING "/' -e 's/$$/"/' -e 1q \
	  >> config.h
	cat local.h config.X \
	  | egrep '^#define[ 	]*MASKTYPE' \
	  | sed -e 's/TYPE[ 	]*/TYPE_STRING "/' -e 's/$$/"/' -e 1q \
	  >> config.h

#	Create a sample local.h if no such file currently exists
local.h:
	set +e; [ -r local.h ]  ||  cp local.h.samp local.h

msgs.h:	config.sh FRC
	@. ./config.sh; \
	  set $(SHELLDEBUG); \
	  set +e; \
	  if [ -r languages/$$MSGLANG/msgs.h ]; then \
	    msgs=languages/$$MSGLANG/msgs.h; \
	  else \
	    msgs=languages/english/msgs.h; \
	  fi; \
	  if cmp -s msgs.h $$msgs; then \
	    :; \
	  else \
	    set -x; \
	    rm -f msgs.h; ln $$msgs msgs.h  ||  cp $$msgs msgs.h; \
	  fi

FRC:

tags:	config.h *.[chy]
	ctags -w -t *.[chy]
	sed -e s/config.h/config.X/ tags > ntags
	mv ntags tags

TAGS:	config.h *.[chy]
	etags *.[chy]
	sed -e s/config.h/config.X/ TAGS > NTAGS
	mv NTAGS TAGS

lint:	languages/*/msgs.h
lint:	config.sh config.h ispell.h proto.h *.[cy]
	@. ./config.sh; \
	  $$LINT $$LINTFLAGS ispell.c correct.c defmt.c dump.c good.c \
	    hash.c lookup.c makedent.c tgood.c term.c tree.c xgets.c; \
	  $$YACC parse.y; \
	  $$LINT $$LINTFLAGS buildhash.c hash.c makedent.c y.tab.c; \
	  $$LINT $$LINTFLAGS icombine.c makedent.c y.tab.c; \
	  $$LINT $$LINTFLAGS ijoin.c fields.c
	@rm -f y.tab.c

clean:	config.sh clean-languages
	@. ./config.sh; \
	  set -x; \
	  rm -f $$DEFHASH $$FOREIGNHASHES
	rm -f *.o core a.out mon.out hash.out y.tab.c *.stat *.cnt \
		config.h msgs.h
	rm -f buildhash findaffix tryaffix ispell icombine ijoin \
		munchlist subset sq unsq zapdups ispell.1 ispell.info

clean-languages:
	$(MAKE) LANGUAGE_TARGET=clean SHELLDEBUG=$(SHELLDEBUG) language-subdirs

realclean veryclean:	clean dictclean
	rm -f config.sh

#
#	The following dependency can be executed when ispell is unpacked,
#	to unpack the dictionaries.
#
unpack:	unsq
	$(MAKE) LANGUAGE_TARGET=unpack SHELLDEBUG=$(SHELLDEBUG) \
	  language-subdirs

unpacked:
	$(MAKE) SHELLDEBUG=$(SHELLDEBUG) unpack
	touch unpacked

#
#	The following target allows you to clean out the leftover raw
#	files gotten from unpacking the kit.  It makes sure that you
#	have the combined files first, so it may take a little while
#	to run.
#
kitclean:	unsq
	$(MAKE) LANGUAGE_TARGET=kitclean SHELLDEBUG=$(SHELLDEBUG) \
	  language-subdirs

#
#	The following target allows you to clean out the combined
#	dictionary files.  For safety, so you don't lose your files,
#	it makes sure that there is something to work from, but it can
#	only be so smart, so be careful!
#
dictclean:
	$(MAKE) LANGUAGE_TARGET=dictclean SHELLDEBUG=$(SHELLDEBUG) \
	  language-subdirs

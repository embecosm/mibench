###############################################################################
#                                                                             #
#  Make file for installing Basser Lout Version 3.24                          #
#                                                                             #
#  Jeffrey H. Kingston                                                        #
#  24 June 2000                                                               #
#                                                                             #
#     make prg2lout     Compile a small auxiliary program called prg2lout     #
#     make lout         Compile the Lout source                               #
#     make install      Install the Lout and prg2lout binaries and libraries  #
#     make installman   Install the Lout and prg2lout manual entries          #
#     make installdoc   Install the Lout documentation                        #
#     make installfr    Install French error messages (optional)              #
#     make installde    Install German error messages (optional)              #
#     make clean        Remove compilation temporaries                        #
#     make uninstall    Undo the effect of make install, installman,          #
#                       installdoc, installfr, and installde                  #
#     make restart      Undo everything except changes to this makefile,      #
#                       ready for a fresh start.                              #
#                                                                             #
#  Most installations of Lout should require only the following steps.  If    #
#  something goes wrong, you can start again with "make restart".  Please     #
#  carry out all the steps, in exactly the order given.  Believe me, it       #
#  will be much faster than doing it any other way.                           #
#                                                                             #
#  (1) Set exactly one of the following macros defined below to 1 and the     #
#      others all to 0, to indicate the operating system under which the      #
#      Lout binary is to run.  At present OSUNIX and OSDOS work but OSMAC     #
#      doesn't work.                                                          #
#                                                                             #
#         OSUNIX   Unix in all its flavours, including Linux.                 #
#         OSDOS    MS-DOS etc. ("rb" and "wb" file access modes where needed) #
#         OSMAC    Macintosh                                                  #
#                                                                             #
#  (2) If you want to install Lout with debugging on for some reason, for     #
#      example if a guru has asked you to do this in the course of tracking   #
#      down some problem with Lout, then set these two macros as follows:     #
#                                                                             #
#         DEBUGGING = 1                                                       #
#         TRACING = -g                                                        #
#                                                                             #
#      Lout will run a bit slower and its binary will be a bit larger if      #
#      you do this.  The normal, non-debugging values are                     #
#                                                                             #
#         DEBUGGING = 0                                                       #
#         TRACING =                                                           #
#                                                                             #
#      These should appear like this below.                                   #
#                                                                             #
#  (3) This may be the trickiest step of all.  On some systems, for example   #
#      NT under Visual C++, Lout's use of the system ftell() call causes      #
#      a problem.  This problem typically manifests itself on the *second*    #
#      pass over a large document such as the Lout User's Guide: a message    #
#      such as "error in database file" is printed and Lout aborts.  The      #
#      precise error may vary but should say something about a problem with   #
#      a database file.                                                       #
#                                                                             #
#      If this problem occurs you can probably fix it by changing the value   #
#      of DBFIX below to 1.  Many thanks to Valeriy E. Ushakov for this fix.  #
#                                                                             #
#         Systems requiring DBFIX = 0      Systems requiring DBFIX = 1        #
#         -------------------------------------------------------------       #
#         Unix                             NT/Visual C++                      #
#         Cygnus gnuwin32 gcc              djgpp                              #
#                                          Mingw32                            #
#                                                                             #
#  (4) Set the USESTAT macro defined below to 1 if the system you are         #
#      compiling onto has the stat() file status system call.  If you are     #
#      unsure, or know it doesn't, set USESTAT to 0.  The stat() call,        #
#      if used, will allow Lout to determine the time of last change          #
#      of database index files and rebuild them automatically if required.    #
#                                                                             #
#  (5) Set the SAFEDFT macro defined below to 1 if you want safe execution    #
#      (i.e. disabling calls to system()) to be the default behaviour.  You   #
#      can always specify safe or unsafe execution by means of the -S and     #
#      -U options to lout when processing a document; SAFEDFT means that      #
#      -S rather than -U is the default behaviour.  Unsafe execution is       #
#      required when formatting computer programs, so if in doubt, do not     #
#      change the value of SAFEDFT.                                           #
#                                                                             #
#  (6) Set the following four macros defined below to appropriate values:     #
#                                                                             #
#      BINDIR    Directory where Lout's binary goes.  This directory is       #
#                assumed to exist.                                            #
#                                                                             #
#      LIBDIR    Directory where Lout's libraries go.  This directory will    #
#                be created (but its parent must exist already).              #
#                                                                             #
#      DOCDIR    Directory where the documents describing the Lout system     #
#                (written in Lout) go.  This directory will be created (but   #
#                its parent must exist already).                              #
#                                                                             #
#      MANDIR    Directory where the lout and prg2lout online manual entries  #
#                (in nroff -man) go.  This directory is assumed to exist.     #
#                                                                             #
#  (7) Set the following two macros defined below to appropriate values.      #
#      I strongly recommend CHARIN=1 and CHAROUT=0 for all sites (English     #
#      and non-English language).  This way we get a truly international      #
#      standard in which everyone has access to accented characters, yet      #
#      Lout's output is in the strict 7-bit ASCII that is recommended in      #
#      the PostScript manual.                                                 #
#                                                                             #
#      CHARIN    This macro determines the assignment of characters in Lout   #
#                source files to character classes by the lexical analyser.   #
#                That is, it determines which characters are letters, which   #
#                is the comment character, etc.  Supported values are:        #
#                                                                             #
#                0  For English language only ASCII installations             #
#                                                                             #
#                1  For installations using the ISO-LATIN-1 character set     #
#                   (adds accented letters to the LETTER character class)     #
#                                                                             #
#                Lout will accept any 8-bit character except '\0'; CHARIN     #
#                does not determine the acceptability of any character, just  #
#                its class.                                                   #
#                                                                             #
#      CHAROUT   This macro determines the format of strings of literal       #
#                characters in the PostScript output.  Currently supported    #
#                values are:                                                  #
#                                                                             #
#                0  Every output character will be printable ASCII            #
#                                                                             #
#                1  Every output character will be printable ISO-LATIN-1      #
#                                                                             #
#                The output will be valid PostScript irrespective of the      #
#                value given to CHAROUT, which may be set independently of    #
#                CHARIN.  It just determines which characters are printed     #
#                as \ddd escape sequences and which are printed as one-byte   #
#                literal characters.                                          #
#                                                                             #
#  (8) Set macro USELOC to one of the following values, NOT TO A LOCALE.      #
#                                                                             #
#      0         Lout's error messages will always appear in English, and no  #
#                source code related to locales will be executed (although    #
#                file <locale.h> will be read for collation stuff).           #
#                                                                             #
#      1         Lout's error messages may appear in languages other than     #
#                English, depending on the current locale.  The Lout source   #
#                will be compiled including <locale.h>, <nl_types.h>, and     #
#                calls to setlocale(), catopen(), catgets(), and catclose()   #
#                                                                             #
#      If you choose to set USELOC to 1, you now need to set one or more of   #
#      these macros:                                                          #
#                                                                             #
#      LOC_FR    If you want French language error messages, set this macro   #
#                to your French locale name, i.e. to the value that you       #
#                expect setlocale(LC_MESSAGES, "") to return when you want    #
#                to get French language error messages                        #
#                                                                             #
#      LOC_DE    If you want German language error messages, set this macro   #
#                to your German locale name, i.e. to the value that you       #
#                expect setlocale(LC_MESSAGES, "") to return when you want    #
#                to get German language error messages                        #
#                                                                             #
#      For error messages in other languages, consult ./locale/README.        #
#                                                                             #
#  (9) Set macro COLLATE to either 0 or 1.  If you set it to 1, Lout will     #
#      use the strcoll() routine by default when sorting alphabetically       #
#      (e.g. when sorting indexes), otherwise Lout will sort by default       #
#      based on the ISO codes of the characters.  This default setting may    #
#      be changed during individual runs of Lout by the -l and -L flags.      #
#                                                                             #
# (10) Execute "make prg2lout".  This will compile the prg2lout program,      #
#      leaving its binary in this directory.  Other directories unchanged.    #
#                                                                             #
# (11) If you want to be able to produce compressed PDF files, as opposed to  #
#      uncompressed ones, you need to:                                        #
#                                                                             #
#      (a) obtain the zlib compression library from                           #
#          <http://www.cdrom.com/pub/infozip/zlib/>.                          #
#                                                                             #
#      (b) decompress the zlib source files using gunzip and/or tar and       #
#          then build the library by issuing the "make zlib.a" command        #
#          whilst in the zlib directory. If you want to test the library,     #
#          you should use the "make test" command (which also builds the      #
#          library).                                                          #
#                                                                             #
#      (c) set the PDF_COMPRESSION variable below to 1                        #
#                                                                             #
#      (d) set the ZLIB variable to the path of the libz.a file. For example: #
#          ZLIB = /usr/cs3/vtan/lout/lout.3.11/zlib-1.1.1/libz.a              #
#                                                                             #
#      (e) set the ZLIBPATH variable to the path of the zlib directory with   #
#          -I in front. For example:                                          #
#          ZLIBPATH = -I/usr/cs3/vtan/lout/lout.3.11/zlib-1.1.1/              #
#                                                                             #
#      If you don't want zlib support or cannot obtain it or cannot use it,   #
#      leave the PDF_COMPRESSION, ZLIB, and ZLIBPATH variables as they are.   #
#                                                                             #
# (12) Execute "make lout".  This will compile the Lout source, leaving the   #
#      binary in this directory.  No changes are made in other directories.   #
#                                                                             #
# (13) This makefile assumes that Lout is not installed on your system        #
#      already.  If you do have an earlier version of Lout installed,         #
#      the simplest way to get rid of it is to type "make uninstall" now.     #
#      Of course, this is assuming that the old version was installed in the  #
#      same directories as where you are about to install the new version.    #
#                                                                             #
# (14) Execute "make install".  This will do the following things:            #
#                                                                             #
#      (a) It will copy the lout and prg2lout binaries into $(BINDIR);        #
#                                                                             #
#      (b) It will create $(LIBDIR) and copy all the library files into it;   #
#                                                                             #
#      (c) It will perform an initializing "lout -x" run.  This run will      #
#          do the following checks and initializations:                       #
#                                                                             #
#          (i)   It will read all the hyphenation (.lh) files mentioned       #
#                in file $(LIBDIR)/include/langdefs, check them, and build    #
#                the packed (.lp) versions;                                   #
#                                                                             #
#          (ii)  It will read and check the four standard database            #
#                (.ld) files in directory $(LIBDIR)/data, and build           #
#                the corresponding database index (.li) files.                #
#                                                                             #
#      (d) It will change the mode of the files created in (c) to be          #
#          publicly readable, just in case they weren't created that way.     #
#                                                                             #
#      It is good to build the various files during installation because      #
#      later runs will not have write permission in the library directories.  #
#                                                                             #
# (15) Execute "make installman".  This installs the manual entries for lout  #
#      and prg2lout into directory $(MANDIR), which is assumed to exist.      #
#      These entries are troff files; plain text versions are also available  #
#      in directory ./man if you need them (install them yourself).           #
#                                                                             #
# (16) Execute "make installdoc".  This creates directory $(DOCDIR) and       #
#      copies the Lout documentation into it.                                 #
#                                                                             #
# (17) If you want French error messages, execute "make installfr" now.       #
#      If you want German error messages, execute "make installde" now.       #
#      These commands compile the error messages files into packed forms      #
#      using the gencat command, and store them in $(LIBDIR)/locale.          #
#                                                                             #
# (18) Execute "make clean".  This cleans up this directory.                  #
#                                                                             #
# (19) If the usual size of a piece of paper at your site is not A4, you      #
#      might like to now change the default value of the @PageType option     #
#      on line 59 of file $(LIBDIR)/include/dsf:                              #
#                                                                             #
#          named @PageType { A4 @OrIfPlain Other }                            #
#                                                                             #
#      This is saying that the page type is to be A4 by default, unless       #
#      plain text output is in effect (lout -p), in which case the page       #
#      type is Other, which means that the page dimensions come from the      #
#      @PageWidth and @PageHeight options.  Just change the A4, not the       #
#      rest.  You can find the list of known page types, alternative to A4,   #
#      in the User's Guide, and also at line 640 in $(LIBDIR)/include/dsf.    #
#                                                                             #
# (20) If the usual language at your site is not English, you might like to   #
#      now change the default value of the @InitialLanguage option on line    #
#      255 of file $(LIBDIR)/include/bsf.  This will mean that by default     #
#      the date and words like Chapter and July will appear in a different    #
#      language, and hyphenation will be carried out according to patterns    #
#      designed for that language.  You can find the list of known languages  #
#      in the User's Guide, or in file $(LIBDIR)/include/langdefs; if yours   #
#      is not on the list, let me know and we can work together to add it.    #
#      This has nothing to do with locales and USELOC.                        #
#                                                                             #
#  Mail jeff@cs.usyd.edu.au if you have any problems.                         #
#                                                                             #
###############################################################################

OSUNIX  = 1
OSDOS   = 0
OSMAC   = 0

DBFIX   = 0

USESTAT = 1
SAFEDFT = 0

DEBUGGING = 0
TRACING =

# DEBUGGING = 1
# TRACING = -g

BINDIR	= /usr/staff/jeff/bin
LIBDIR	= /usr/staff/jeff/lout.lib
DOCDIR	= /usr/staff/jeff/lout.doc
MANDIR	= /usr/staff/jeff/lout.man

LIBFONT = font
LIBMAPS = maps
LIBINCL = include
LIBDATA = data
LIBHYPH = hyph
LIBLOCA = locale

CHARIN	= 1
CHAROUT	= 0

USELOC	= 1
LOC_FR	= fr
LOC_DE	= de

COLLATE	= 1

PDF_COMPRESSION	= 0
ZLIB		=
ZLIBPATH	=

CC	= gcc
LD	= gcc -static

RCOPY	= cp -r

COPTS	= -ansi -pedantic -Wall -O3
# COPTS	= -ansi -pedantic -Wall

CFLAGS	= -DOS_UNIX=$(OSUNIX)					\
	  -DOS_DOS=$(OSDOS)					\
	  -DOS_MAC=$(OSMAC)					\
	  -DDB_FIX=$(DBFIX)					\
	  -DUSE_STAT=$(USESTAT)					\
	  -DSAFE_DFT=$(SAFEDFT)					\
	  -DCOLLATE=$(COLLATE)					\
	  -DLIB_DIR=\"$(LIBDIR)\"				\
	  -DFONT_DIR=\"$(LIBFONT)\"				\
	  -DMAPS_DIR=\"$(LIBMAPS)\"				\
	  -DINCL_DIR=\"$(LIBINCL)\"				\
	  -DDATA_DIR=\"$(LIBDATA)\"				\
	  -DHYPH_DIR=\"$(LIBHYPH)\"				\
	  -DLOCALE_DIR=\"$(LIBLOCA)\"				\
	  -DCHAR_IN=$(CHARIN)					\
	  -DCHAR_OUT=$(CHAROUT)					\
	  -DLOCALE_ON=$(USELOC)					\
	  -DASSERT_ON=1 $(COPTS)				\
	  -DDEBUG_ON=$(DEBUGGING)				\
	  $(TRACING)						\
	  -DPDF_COMPRESSION=$(PDF_COMPRESSION)			\
	  $(ZLIBPATH)

OBJS	= z01.o z02.o z03.o z04.o z05.o z06.o z07.o z08.o	\
	  z09.o z10.o z11.o z12.o z13.o z14.o z15.o z16.o	\
	  z17.o z18.o z19.o z20.o z21.o z22.o z23.o z24.o	\
	  z25.o z26.o z27.o z28.o z29.o z30.o z31.o z32.o	\
	  z33.o z34.o z35.o z36.o z37.o z38.o z39.o z40.o	\
	  z41.o z42.o z43.o z44.o z45.o z46.o z47.o z48.o	\
	  z49.o z50.o z51.o

lout:	$(OBJS)
	$(LD) -o lout $(OBJS) $(ZLIB) -lm
	chmod a+x lout

$(OBJS): externs.h

externs.h:

prg2lout:	prg2lout.c
	$(CC) $(COPTS) -o prg2lout prg2lout.c
	chmod a+x prg2lout

install: lout prg2lout
	@echo ""
	@echo "(a) Installing lout and prg2lout binaries into BINDIR $(BINDIR)"
	cp lout $(BINDIR)/lout
	chmod 755 $(BINDIR)/lout
	cp prg2lout $(BINDIR)/prg2lout
	chmod 755 $(BINDIR)/prg2lout
	@echo ""
	@echo "(b) Installing library files into LIBDIR $(LIBDIR)"
	mkdir $(LIBDIR)
	chmod 755 $(LIBDIR)
	@echo ""
	mkdir $(LIBDIR)/$(LIBINCL)
	chmod 755 $(LIBDIR)/$(LIBINCL)
	cp include/* $(LIBDIR)/$(LIBINCL)
	chmod 644 $(LIBDIR)/$(LIBINCL)/*
	@echo ""
	mkdir $(LIBDIR)/$(LIBDATA)
	chmod 755 $(LIBDIR)/$(LIBDATA)
	cp data/* $(LIBDIR)/$(LIBDATA)
	chmod 644 $(LIBDIR)/$(LIBDATA)/*
	@echo ""
	mkdir $(LIBDIR)/$(LIBHYPH)
	chmod 755 $(LIBDIR)/$(LIBHYPH)
	cp hyph/* $(LIBDIR)/$(LIBHYPH)
	chmod 644 $(LIBDIR)/$(LIBHYPH)/*
	@echo ""
	mkdir $(LIBDIR)/$(LIBFONT)
	chmod 755 $(LIBDIR)/$(LIBFONT)
	cp font/* $(LIBDIR)/$(LIBFONT)
	chmod 644 $(LIBDIR)/$(LIBFONT)/*
	@echo ""
	mkdir $(LIBDIR)/$(LIBMAPS)
	chmod 755 $(LIBDIR)/$(LIBMAPS)
	cp maps/* $(LIBDIR)/$(LIBMAPS)
	chmod 644 $(LIBDIR)/$(LIBMAPS)/*
	@echo ""
	mkdir $(LIBDIR)/$(LIBLOCA)
	chmod 755 $(LIBDIR)/$(LIBLOCA)
	@echo ""
	@echo "(c) Initializing run (should be silent, no errors expected)"
	$(BINDIR)/lout -x -s $(LIBDIR)/$(LIBINCL)/init
	@echo ""
	@echo "(d) Changing mode of files just created by initializing run"
	chmod 644 $(LIBDIR)/$(LIBDATA)/*
	chmod 644 $(LIBDIR)/$(LIBHYPH)/*

installman:
	@echo ""
	@echo "Installing manual entries into MANDIR $(MANDIR)"
	sed -e "s@<BINDIR>@$(BINDIR)@" -e "s@<LIBDIR>@$(LIBDIR)@"	\
	    -e "s@<DOCDIR>@$(DOCDIR)@" -e "s@<MANDIR>@$(MANDIR)@"	\
	man/lout.1 > $(MANDIR)/lout.1
	chmod 644 $(MANDIR)/lout.1
	cp man/prg2lout.1 $(MANDIR)/prg2lout.1
	chmod 644 $(MANDIR)/prg2lout.1

installdoc:
	@echo ""
	@echo "Creating DOCDIR $(DOCDIR) and copying documentation into it"
	$(RCOPY) doc $(DOCDIR)
	chmod 755 $(DOCDIR)
	chmod 755 $(DOCDIR)/*
	chmod 644 $(DOCDIR)/*/*

installfr:
	@echo ""
	@echo "Putting French error messages into $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)"
	mkdir $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)
	chmod 755 $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)
	mkdir $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)/LC_MESSAGES
	chmod 755 $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)/LC_MESSAGES
	cp locale/msgs.fr $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)/LC_MESSAGES/msgs.$(LOC_FR)
	gencat $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)/LC_MESSAGES/errors.$(LOC_FR)	\
	       $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)/LC_MESSAGES/msgs.$(LOC_FR)
	chmod 644 $(LIBDIR)/$(LIBLOCA)/$(LOC_FR)/LC_MESSAGES/*

installde:
	@echo ""
	@echo "Putting German error messages into $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)"
	mkdir $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)
	chmod 755 $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)
	mkdir $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)/LC_MESSAGES
	chmod 755 $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)/LC_MESSAGES
	cp locale/msgs.de $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)/LC_MESSAGES/msgs.$(LOC_DE)
	gencat $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)/LC_MESSAGES/errors.$(LOC_DE)	\
	       $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)/LC_MESSAGES/msgs.$(LOC_DE)
	chmod 644 $(LIBDIR)/$(LIBLOCA)/$(LOC_DE)/LC_MESSAGES/*

uninstall:
	-rm -f  $(BINDIR)/lout $(BINDIR)/prg2lout
	-rm -fr $(LIBDIR)
	-rm -fr $(DOCDIR)
	-rm -f  $(MANDIR)/lout.1 $(MANDIR)/prg2lout.1

clean:	
	-rm -f lout prg2lout *.o

restart:	clean uninstall

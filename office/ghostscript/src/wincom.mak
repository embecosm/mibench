#    Copyright (C) 1991-1997 Aladdin Enterprises.  All rights reserved.
# 
# This file is part of Aladdin Ghostscript.
# 
# Aladdin Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author
# or distributor accepts any responsibility for the consequences of using it,
# or for whether it serves any particular purpose or works at all, unless he
# or she says so in writing.  Refer to the Aladdin Ghostscript Free Public
# License (the "License") for full details.
# 
# Every copy of Aladdin Ghostscript must include a copy of the License,
# normally in a plain ASCII text file named PUBLIC.  The License grants you
# the right to copy, modify and redistribute Aladdin Ghostscript, but only
# under certain conditions described in the License.  Among other things, the
# License requires that the copyright notice and this notice be preserved on
# all copies.

# Common makefile section for 32-bit MS Windows.

# Do not use conditionals in this makefile, because they are
# different in NMAKE (!if) and WMAKE (!ifeq).


# Note that built-in libpng and zlib aren't available.

SHARE_LIBPNG=0
SHARE_ZLIB=0

# Define the platform name.

PLATFORM=mswin32_

# Define the ANSI-to-K&R dependency.  Borland C, Microsoft C and
# Watcom C all accept ANSI syntax, but we need to preconstruct ccf32.tr 
# to get around the limit on the maximum length of a command line.

AK=ccf32.tr

# Define the syntax for command, object, and executable files.

CMD=.bat
O=-o
OBJ=obj
XE=.exe
XEAUX=.exe

# Define generic commands.

# We have to use a batch file for the equivalent of cp,
# because the DOS COPY command copies the file write time, like cp -p.
CP_=call cp.bat
RM_=erase
RMN_=call rm.bat

# Define the generic compilation flags.

PLATOPT=

INTASM=
PCFBASM=

# Define the files to be removed by `make clean'.
# nmake expands macros when encountered, not when used,
# so this must precede the !include statements.

BEGINFILES=gs*.res gs*.ico ccf32.tr\
   $(GSDLL).dll $(GSCONSOLE).exe\
   $(BEGINFILES2)

# Include the generic makefiles.
!include version.mak
!include gs.mak
!include lib.mak
!include jpeg.mak
!include libpng.mak
!include zlib.mak
!include devs.mak
!include int.mak

# -------------------------- Auxiliary programs --------------------------- #

$(ECHOGS_XE): echogs.c
	$(CCAUX_SETUP)
	$(CCAUX) echogs.c $(CCAUX_TAIL)

# $(GENARCH_XE) is in individual (compiler-specific) makefiles.

$(GENCONF_XE): genconf.c $(stdpre_h)
	$(CCAUX_SETUP)
	$(CCAUX) genconf.c $(CCAUX_TAIL)

$(GENINIT_XE): geninit.c $(stdio__h) $(string__h)
	$(CCAUX_SETUP)
	$(CCAUX) geninit.c $(CCAUX_TAIL)

# No special gconfig_.h is needed.
# Assume `make' supports output redirection.
gconfig_.h: $(MAKEFILE)
	echo /* This file deliberately left blank. */ >gconfig_.h

gconfigv.h: $(MAKEFILE) $(ECHOGS_XE)
	$(EXP)echogs -w gconfigv.h -x 23 define USE_ASM -x 2028 -q $(USE_ASM)-0 -x 29
	$(EXP)echogs -a gconfigv.h -x 23 define USE_FPU -x 2028 -q $(FPU_TYPE)-0 -x 29
	$(EXP)echogs -a gconfigv.h -x 23 define EXTEND_NAMES 0$(EXTEND_NAMES)


# -------------------------------- Library -------------------------------- #

# The Windows Win32 platform

mswin32__=gp_mswin.$(OBJ) gp_win32.$(OBJ) gp_nofb.$(OBJ) gp_ntfs.$(OBJ)
mswin32_.dev: $(mswin32__)
        $(SETMOD) mswin32_ $(mswin32__)
        $(ADDMOD) mswin32_ -iodev wstdio

gp_mswin.$(OBJ): gp_mswin.c $(AK) gp_mswin.h \
 $(ctype__h) $(dos__h) $(malloc__h) $(memory__h) $(stdio__h) $(string__h) $(windows__h) \
 $(gx_h) $(gp_h) $(gpcheck_h) $(gserrors_h) $(gsexit_h) $(gxiodev_h) $(stream_h)

# ----------------------------- Main program ------------------------------ #

CCBEGIN=$(CCC) *.c

ICONS=gsgraph.ico gstext.ico

GS_ALL=$(INT_ALL) $(INTASM)\
  $(LIB_ALL) $(LIBCTR) lib.tr $(ld_tr) $(GSDLL).res $(GSDLL).def $(ICONS)

# Make the icons from their text form.

gsgraph.ico: gsgraph.icx echogs$(XE)
        echogs -wb gsgraph.ico -n -X -r gsgraph.icx

gstext.ico: gstext.icx echogs$(XE)
        echogs -wb gstext.ico -n -X -r gstext.icx

# resources for short EXE loader (no dialogs)
$(GS).res: dwmain.rc dwmain.h $(ICONS)
	$(RCOMP) -i$(INCDIR) -r -fo$(GS).res dwmain.rc

# resources for main program (includes dialogs)
$(GSDLL).res: gsdll32.rc gp_mswin.h $(ICONS)
	$(RCOMP) -i$(INCDIR) -r -fo$(GSDLL).res gsdll32.rc


# Modules for small EXE loader.

DWOBJ=dwdll.obj dwimg.obj dwmain.obj dwtext.obj gscdefs.obj

dwdll.obj: dwdll.cpp dwdll.h gsdll.h
	$(CPP) $(COMPILE_FOR_EXE) -c dwdll.cpp

dwimg.obj: dwimg.cpp dwmain.h dwdll.h dwtext.h dwimg.h gscdefs.h gsdll.h
	$(CPP) $(COMPILE_FOR_EXE) -c dwimg.cpp

dwmain.obj: dwmain.cpp dwdll.h gscdefs.h gsdll.h
	$(CPP) $(COMPILE_FOR_EXE) -c dwmain.cpp

dwtext.obj: dwtext.cpp dwtext.h
	$(CPP) $(COMPILE_FOR_EXE) -c dwtext.cpp

# Modules for big EXE

DWOBJNO = dwnodll.obj dwimg.obj dwmain.obj dwtext.obj

dwnodll.obj: dwnodll.cpp dwdll.h gsdll.h
	$(CPP) $(COMPILE_FOR_EXE) -c dwnodll.cpp

# Compile gsdll.c, the main program of the DLL.

gsdll.$(OBJ): gsdll.c gsdll.h $(ghost_h)

# Modules for console mode EXEs

OBJC=dwmainc.obj dwdllc.obj gscdefs.obj
OBJCNO=dwmainc.obj dwnodllc.obj

dwmainc.obj: dwmainc.cpp dwmain.h dwdll.h gscdefs.h gsdll.h
	$(CPP) $(COMPILE_FOR_CONSOLE_EXE) -c dwmainc.cpp

dwdllc.obj: dwdll.cpp dwdll.h gsdll.h
	$(CPP) $(COMPILE_FOR_CONSOLE_EXE) -c $(CCOBJNAME)dwdllc.obj dwdll.cpp

dwnodllc.obj: dwnodll.cpp dwdll.h gsdll.h
	$(CPP) $(COMPILE_FOR_CONSOLE_EXE) -c $(CCOBJNAME)dwnodllc.obj dwnodll.cpp

# end of makefile

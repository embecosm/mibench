#    Copyright (C) 1997 Aladdin Enterprises.  All rights reserved.
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

# msvccom.mak
# Common makefile section for Microsoft Visual C++ 4.1,
# Windows NT or Windows 95 platform.
# Created 1997-05-22 by L. Peter Deutsch from msvc4/5 makefiles.

# Define the current directory prefix and shell invocations.

D=\#

EXPP=
SH=
SHP=

# Define the arguments for genconf.

CONFILES=-p %%s -o $(ld_tr) -l lib.tr

# Define the generic compilation flags.

PLATOPT=

INTASM=
PCFBASM=

# Make sure we get the right default target for make.

dosdefault: default

# Define the compilation flags.

!if "$(CPU)"=="i386"
!if $(CPU_TYPE)>500
CPFLAGS=/G5 /QI0f
!else if $(CPU_TYPE)>400
CPFLAGS=/G4 /QI0f
!else
CPFLAGS=/G3 /QI0f
!endif

!if $(FPU_TYPE)>0
FPFLAGS=/FPi87
!else
FPFLAGS=
!endif
!endif

!if "$(CPU)"=="ppc"
!if $(CPU_TYPE)>=620
CPFLAGS=/QP620
!else if $(CPU_TYPE)>=604
CPFLAGS=/QP604
!else
CPFLAGS=/QP601
!endif

FPFLAGS=
!endif

!if "$(CPU)"=="alpha"
# *** alpha *** This needs fixing
CPFLAGS=
FPFLAGS=
!endif

!if $(NOPRIVATE)!=0
CP=/DNOPRIVATE
!else
CP=
!endif

!if $(DEBUG)!=0
CD=/DDEBUG /Gt128
!else
CD=
!endif

!if $(TDEBUG)!=0
CT=/f /Zi /Od
LCT=/DEBUG
COMPILE_FULL_OPTIMIZED=    # no optimization when debugging
COMPILE_WITH_FRAMES=    # no optimization when debugging
COMPILE_WITHOUT_FRAMES=    # no optimization when debugging
!else
CT=
LCT=
COMPILE_FULL_OPTIMIZED=/O2
COMPILE_WITH_FRAMES=
COMPILE_WITHOUT_FRAMES=/Oy
!endif

!if $(DEBUG)!=0 || $(TDEBUG)!=0
CS=/Ge
!else
CS=/Gs
!endif

# Specify output object name
CCOBJNAME=-Fo

# Specify function prolog type
COMPILE_FOR_DLL=
COMPILE_FOR_EXE=
COMPILE_FOR_CONSOLE_EXE=


GENOPT=$(CP) $(CD) $(CT) $(CS) /W2 /nologo

CCFLAGS=$(PLATOPT) $(FPFLAGS) $(CPFLAGS) $(CFLAGS) $(XCFLAGS)
CC=$(COMP) /c $(CCFLAGS) @ccf32.tr
CPP=$(COMPCPP) /c $(CCFLAGS) @ccf32.tr
!if $(MAKEDLL)
WX=$(COMPILE_FOR_DLL)
!else
WX=$(COMPILE_FOR_EXE)
!endif
CCC=$(CC) $(WX) $(COMPILE_FULL_OPTIMIZED)
CCD=$(CC) $(WX) $(COMPILE_WITH_FRAMES)
CCINT=$(CCC)
CCCF=$(CCC)
CCLEAF=$(CCC) $(COMPILE_WITHOUT_FRAMES)

# Compiler for auxiliary programs

CCAUX=$(COMPAUX) /I$(INCDIR) /O


# Define the generic compilation rules.

.c.obj:
	$(CCC) $<

.cpp.obj:
	$(CPP) $<

# Define the files to be removed by `make clean'.
# nmake expands macros when encountered, not when used,
# so this must precede the !include statements.

BEGINFILES2=gsdll32.exp gsdll32.ilk gsdll32.pdb gsdll32.lib\
   gswin32.exp gswin32.ilk gswin32.pdb gswin32.lib\
   gswin32c.exp gswin32c.ilk gswin32c.pdb gswin32c.lib


# Include the generic makefiles.

!include wincom.mak


# -------------------------- Auxiliary programs --------------------------- #

ccf32.tr: $(MAKEFILE) makefile
	echo $(GENOPT) /I$(INCDIR) -DCHECK_INTERRUPTS -D_Windows -D__WIN32__ > ccf32.tr

$(GENARCH_XE): genarch.c $(stdpre_h) $(iref_h) ccf32.tr
	$(CCAUX_SETUP)
	$(CCAUX) @ccf32.tr genarch.c $(CCAUX_TAIL)

# -------------------------------- Library -------------------------------- #

# See wincom.mak

# ----------------------------- Main program ------------------------------ #

LIBCTR=libc32.tr

$(LIBCTR): $(MAKEFILE) $(ECHOGS_XE)
        echogs -w $(LIBCTR) $(LIBDIR)\shell32.lib
        echogs -a $(LIBCTR) $(LIBDIR)\comdlg32.lib
        echogs -a $(LIBCTR) $(LIBDIR)\gdi32.lib
        echogs -a $(LIBCTR) $(LIBDIR)\user32.lib
        echogs -a $(LIBCTR) $(LIBDIR)\winspool.lib

!if $(MAKEDLL)
# The graphical small EXE loader
$(GS_XE): $(GSDLL).dll  $(DWOBJ) $(GSCONSOLE).exe
	echo /SUBSYSTEM:WINDOWS > gswin32.rsp
	echo /DEF:dwmain32.def /OUT:$(GS_XE) >> gswin32.rsp
        $(LINK) $(LCT) @gswin32.rsp $(DWOBJ) @$(LIBCTR) $(GS).res
	-del gswin32.rsp

# The console mode small EXE loader
$(GSCONSOLE).exe: $(OBJC) $(GS).res dw32c.def
	echo /SUBSYSTEM:CONSOLE > gswin32.rsp
	echo  /DEF:dw32c.def /OUT:$(GSCONSOLE).exe >> gswin32.rsp
        $(LINK) $(LCT) @gswin32.rsp $(OBJC) @$(LIBCTR) $(GS).res
	-del gswin32.rsp

# The big DLL
$(GSDLL).dll: $(GS_ALL) $(DEVS_ALL) gsdll.$(OBJ) $(GSDLL).res
	echo /DLL /DEF:gsdll32.def /OUT:$(GSDLL).dll > gswin32.rsp
        $(LINK) $(LCT) @gswin32.rsp gsdll @$(ld_tr) $(INTASM) @lib.tr @$(LIBCTR) $(GSDLL).res
	-del gswin32.rsp

!else
# The big graphical EXE
$(GS_XE):   $(GSCONSOLE).exe $(GS_ALL) $(DEVS_ALL) gsdll.$(OBJ) $(DWOBJNO) $(GS).res dwmain32.def
	copy $(ld_tr) gswin32.tr
	echo dwnodll.obj >> gswin32.tr
	echo dwimg.obj >> gswin32.tr
	echo dwmain.obj >> gswin32.tr
	echo dwtext.obj >> gswin32.tr
	echo /DEF:dwmain32.def /OUT:$(GS_XE) > gswin32.rsp
        $(LINK) $(LCT) @gswin32.rsp gsdll @gswin32.tr @$(LIBCTR) $(INTASM) @lib.tr $(GSDLL).res
	-del gswin32.tr
	-del gswin32.rsp

# The big console mode EXE
$(GSCONSOLE).exe:  $(GS_ALL) $(DEVS_ALL) gsdll.$(OBJ) $(OBJCNO) $(GS).res dw32c.def
	copy $(ld_tr) gswin32c.tr
	echo dwnodllc.obj >> gswin32c.tr
	echo dwmainc.obj >> gswin32c.tr
	echo  /SUBSYSTEM:CONSOLE > gswin32.rsp
	echo  /DEF:dw32c.def /OUT:$(GSCONSOLE).exe  >> gswin32.rsp
        $(LINK) $(LCT) @gswin32.rsp gsdll @gswin32c.tr @$(LIBCTR) $(INTASM) @lib.tr $(GS).res
	-del gswin32.rsp
	-del gswin32c.tr
!endif

# end of msvccom.mak

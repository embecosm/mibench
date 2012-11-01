# Makefile for PGP (Unix)
#
#
# This makefile is intended for PGP 2.6.3i. PGP 2.6.3i is based on MIT's
# PGP 2.6.2, but has been modified so that it:
#
#  (1) does not use RSAREF
#  (2) allows you to disable the "legal kludge"
#  (3) corrects a number of bugs present in PGP 2.6.2
#  (4) adds a number of new features without breaking compatibility
#      with older versions
#
# PGP 2.6.3i is probably illegal to use within the USA, so if you're
# physically inside the US, you should do the following:
#
#  (1) Obtain and build a copy of the RSAREF 1.0 library (included with
#      MIT PGP 2.6 and later)
#  (2) Add the -DUSA compiler switch to CFLAGS for your system below
#  (3) Comment out the lines for MPILIB (rsaglue1.o) and uncomment the
#      lines for RSAREF (rsaglue2.o) below
#
# This will give you a version of PGP called 2.6.3, which is identical
# to 2.6.3i, except that it uses RSAREF and the "legal kludge" cannot
# be disabled.
#
#
# CFLAGS options:
#
# -DUSA       to get a version that is legal to use within the US
# -DHIGHFIRST if building PGP on a big-endian system
# -DDEBUG     to include debugging information
# -DNOTERMIO  if your system has no termios
# -DSVR2      for System V release 2
# -DDYN_ALLOC if your compiler does not support large static arrays
# -DSMALL_MEM if your machine has a small memory (required for MSDOS)
# -DIDEA32    if your int's are 32 bits this is probably faster
#
# -DPORTABLE  and
# -DMPORTABLE to build the portable version of the RSA primitives
#             (ie if no optimized asm versions are available)
#
# Define one of:
# -DMERRITT    Merritt's modmult (fast on risc machines)
# -DPEASANT    Russian peasant modulo multiply algorithm
# -DUPTON      use Upton's modmult algorithm
# -DSMITH      use Smith's modmult
# See also the file platform.h for system defaults
#
# If you don't have a working FIONREAD ioctl you must use one of these:
# -DUSE_SELECT to use select() system call
# -DUSE_NBIO   to use non-blocking read()

CFLAGS= -O -DUNIX -DPORTABLE $(BYTEORDER)

# must set byte order for targets "sysv" and "bsd"
# BYTEORDER= -DHIGHFIRST

CC      = cc
LD      = cc            # Link command
LDFLAGS = 
CPP     = $(CC) -E
DBG     = -O
RM      = rm -f

# uncomment this for old versions of make
#MAKE   = make

ASM     = $(CC) -c              # Assembler command

OBJS_EXT=                       # ASM obj. files
LIBS_EXT=                       # Libararies

PROJ    = pgp

default:
	@echo "To build PGP on your flavour of Unix, type:"
	@echo "        make <system>"
	@echo ""
	@echo "where <system> can be:"
	@echo "	386bsd, 3b1, 3b1_asm, aix370, aix386, amix-68k-gcc, apollo,"
	@echo "	apollo-gcc, atari,aux(*), aux-gcc, bsd, bsdgcc, convexos,"
	@echo "	dgux, djgpp, encore, freebsd, hpux-68k(*), hpux-68k-ansi,"
	@echo "	hpux-68k-gcc, hpux-pa(*), hpux-pa-ansi, hpux-pa-gcc,"
	@echo "	hpux9-pa-ansi, irix, irix_asm, isc, isc_asm, linux, linux-68k,"
	@echo "	machten, mach_386, mips-ultrix, netbsd, netbsd_68k, newsasm,"
	@echo "	newsgcc, next, next-intel, next486, nextHP, os2, osf, qnx4,"
	@echo "	riscos501, rs6000, rt_aos4, sco-2.0, sco-3.0, sco-3.0-gcc,"
	@echo "	sco386cc, sgigcc, sgigcc_asm, sinix, solx86gcc, sun386i,"
	@echo "	sun3asm, sun3cc(*), sun3gcc, sun4cc(*), sun4gcc,"
	@echo "	sun4sunos5gcc, sun4acc, sun4sunos5acc, sunspc, sysv, sysvgcc,"
	@echo "	sysv_386, sysv_386_gcc, unixware, vax-ultrix, vax_bsd43,"
	@echo "	xenix286(*), xenix386"
	@echo ""
	@echo "For targets marked with (*) you must first get unproto."
	@echo "See setup.doc for further details."

all: $(PROJ)

# For a pure MPILIB version, uncomment the following lines
RSALIBS =
RSAOBJS = rsaglue1.o

# For an RSAREF version, uncomment the following lines
#RSADIR = ../rsaref
#RSALIBDIR = $(RSADIR)/install/unix
#RSAINCDIR = -I$(RSADIR)/source -I$(RSADIR)/test $(USEMPILIB)
#RSALIBS = $(RSALIBDIR)/rsaref.a
#RSAOBJS = rsaglue2.o

# If you want to use MPILIB as a back end to RSAREF, uncomment this line:
#USEMPILIB = -DUSEMPILIB

# Assembly-language subroutine dependencies

_80386.o:	80386.S
	$(CPP) $(ASMDEF) 80386.S > _80386.s
	$(ASM) -o _80386.o _80386.s
	$(RM) _80386.s

8086.o:	8086.asm
	cp 8086.asm 8086.s
	$(ASM) -o 8086.o 8086.s
	$(RM) 8086.s

_zmatch.o:	zmatch.S
	$(CPP) $(ASMDEF) zmatch.S > _zmatch.s
	$(ASM) -o _zmatch.o _zmatch.s
	$(RM) _zmatch.s

sparc.o:	sparc.S
	$(CPP) $(ASMDEF) sparc.S | grep -v '^# ' > _sparc.s
	$(ASM) -o sparc.o _sparc.s
	$(RM) _sparc.s

mc68020.o:
	$(ASM) -o mc68020.o mc68020.s

ZIPOBJS= zbits.o zdeflate.o zfile_io.o zglobals.o \
	zinflate.o zip.o zipup.o ztrees.o zunzip.o 

OBJ1 =  pgp.o crypto.o keymgmt.o fileio.o \
	mdfile.o more.o armor.o mpilib.o mpiio.o \
	genprime.o rsagen.o random.o idea.o passwd.o \
	md5.o system.o language.o getopt.o keyadd.o \
	config.o keymaint.o charset.o \
	randpool.o noise.o

OBJS =  $(OBJ1) $(ZIPOBJS) $(RSAOBJS) $(OBJS_EXT)
CFLAGS = $(CFLAGS) -I$(RSAINCDIR)

$(PROJ):	$(OBJS)
		$(LD) -o $(PROJ) $(OBJS) $(LDFLAGS) $(LIBS_EXT) $(RSALIBS)

#  Commodore Amiga Running SVR4 2.1p2a with GCC 2.6.0 LER
amix-68k-gcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O2 -DHIGHFIRST -DUNIX \
	-DPORTABLE -DIDEA32"

# Atari-version with gcc 2.5.8
atari:
	$(MAKE) all CC=gcc LD=gcc PROJ=pgp.ttp \
	CFLAGS="$(RSAINCDIR) $(USEMPILIB) -O2 -DATARI -DHIGHFIRST -DPORTABLE \
	-DUNIT32 -DIDEA32"

linux:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O6 -g3 -DUNIX -DLINUX -DIDEA32 -DASM"

arm-linux:
	$(MAKE) all CC=arm-linux-gcc LD=arm-linux-gcc OBJS_EXT="" \
	CFLAGS="$(RSAINCDIR) -O2 -g0 -DUNIX -DLINUX -DIDEA32 -DPORTABLE"

linux-68k:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O2 -DHIGHFIRST -DUNIX -DPORTABLE -DIDEA32"

freebsd:
	$(MAKE) all LD="$(CC) -s" OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O2 -fno-strength-reduce -DUNIX -DIDEA32 -DASM \
	-DMAX_NAMELEN=255"

386bsd:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DASM -DMAX_NAMELEN=255"

netbsd:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DASM -DMAX_NAMELEN=255"

netbsd_68k:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="mc68020.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DHIGHFIRST -DIDEA32 -DMAX_NAMELEN=255"

# Sun 3 with gcc
# change -traditional-cpp to -traditional for gcc < 2.0
sun3gcc:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT=memmove.o \
	CFLAGS="$(RSAINCDIR) -O -traditional-cpp -DUNIX -DHIGHFIRST \
	-DIDEA32 -DPORTABLE"

sun3asm:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="mc68020.o memmove.o" \
	CFLAGS="$(RSAINCDIR) -O -traditional-cpp -DUNIX -DHIGHFIRST -DIDEA32"

# Sun 3 with standard cc: compile with unproto
sun3cc: unproto/cpp
	$(MAKE) all CC=cc LD=cc OBJS_EXT=memmove.o \
	CFLAGS="$(RSAINCDIR) -Qpath unproto -O -DUNIX -DHIGHFIRST \
	-DIDEA32 -DPORTABLE"

# Sun 4 SPARC with gcc (tested with gcc 1.39 and newer, sparc.s can not be
# used with older versions of gcc)
# change -traditional-cpp to -traditional for gcc < 2.0
sun4gcc:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="sparc.o memmove.o" \
	CFLAGS="$(RSAINCDIR) -O -traditional-cpp -DUNIX -DHIGHFIRST -DIDEA32"

# Sun 4 SPARC with standard cc: compile with unproto
sun4cc: unproto/cpp
	$(MAKE) all CC=cc LD=cc OBJS_EXT="sparc.o memmove.o"  \
	CFLAGS="$(RSAINCDIR) -Qpath unproto -Dconst= -O -DUNIX \
	-DHIGHFIRST -DIDEA32"

# Sun 4 using SunPro C compiler
sun4acc:
	$(MAKE) all CC=acc LD=acc CPP="cc -E" OBJS_EXT="sparc.o memmove.o" \
	CFLAGS="-w $(RSAINCDIR) -xO4 -DUNIX -DHIGHFIRST -DIDEA32" \
 
# Sun 4 running Sunos5 (Solaris) using gcc
sun4sunos5gcc:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT=sparc.o \
	CFLAGS="$(RSAINCDIR) -O -traditional-cpp -DSOLARIS -DUNIX -DHIGHFIRST \
	-DIDEA32" ASMDEF=-DSYSV

# Sun 4 running SunOs 5 (Solaris) using SunPro C compiler
sun4sunos5acc:
	$(MAKE) all CC=cc LD=cc \
	CFLAGS="-w $(RSAINCDIR) -xO2 -DSOLARIS -DUNIX -DHIGHFIRST -DIDEA32 \
	-DPORTABLE"

sun386i:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="_80386.o memmove.o" \
	CFLAGS="$(RSAINCDIR) -I. -O -DUNIX -DIDEA32 -DNOTERMIO" \
	ASMDEF=-DSYSV

sunspc:
	$(MAKE) all CC="ccspc -B/1.8.6/sun4 -ansi -w -I/usr/include" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DHIGHFIRST -DUNIT32 \
	-DMERRITT" OBJS_EXT=sparc.o

# x86 running SunOS 5 (Solaris)...
solx86gcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -traditional-cpp -DSOLARIS -DUNIX -DIDEA32 \
        -DPORTABLE"

# Sony newsos v3 for m68k, with gcc
# change -traditional-cpp to -traditional for gcc < 2.0
newsgcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -I. -traditional-cpp -DNOTERMIO -D_BSD \
	-DUNIX -DHIGHFIRST -DIDEA32 -DPORTABLE"

#  with asm
newsasm:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT=mc68020.s CFLAGS="$(RSAINCDIR) -O \
	-I. -traditional-cpp -DNOTERMIO -D_BSD -DUNIX -DHIGHFIRST -DIDEA32"

qnx4:
	$(MAKE) all \
	CFLAGS="$(RSAINCDIR) -3 -O -I. -b -DNO_PARAM_H -DUNIX -DIDEA32 \
	-DPORTABLE -DMPORTABLE $(BYTEORDER) -DMAX_NAMELEN=255"

encore:
	$(MAKE) all CC=gcc LD=gcc CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE \
	-DMPORTABLE -DUSE_NBIO -DIDEA32 -DUPTON -DHIGHFIRST"

sysv:
	$(MAKE) all CPP=/lib/cpp \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_NBIO $(BYTEORDER)"

sysvgcc:
	$(MAKE) all CC=gcc LD=gcc CPP=/lib/cpp \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_NBIO $(BYTEORDER)"

# Optimized version with 80386.S
sysv_386:
	$(MAKE) all CPP="/lib/cpp -DSYSV" OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DUSE_NBIO -DASM"

# gcc tested on ESIX 4.0.3a with gcc 2.5.8
sysv_386_gcc:
	$(MAKE) all CC=gcc LD=gcc CPP="/lib/cpp -DSYSV" \
	OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DUSE_NBIO -DASM"

# Interactive Unix SVR3/386 version 3.2 with gcc
isc:
	$(MAKE) all CC=gcc LD=gcc LDFLAGS="-lcposix -lrpc" OBJS_EXT=memmove.o \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DUSE_NBIO -DPORTABLE \
	-DNO_ITIMER"

isc_asm:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="_80386.o memmove.o" ASMDEF=-DSYSV \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DUSE_NBIO -DNO_ITIMER" \
	LDFLAGS="-lcposix -lrpc"

mach_386:
	$(MAKE) all CC=gcc LD=gcc CPP=/lib/cpp OBJS_EXT=_80386.o \
	CFLAGS="$(RSAINCDIR) -O -I. -DMACH -DUNIX -DIDEA32 -DNOTERMIO"

machten:
	$(MAKE) all CC=gcc LD=gcc CPP=/lib/cpp OBJS_EXT=mc68020.o \
	CFLAGS="$(RSAINCDIR) -O -I. -DMACH -DUNIX -DHIGHFIRST \
	-DIDEA32 -DNOTERMIO"

sco-2.0:
	$(MAKE) all CC=gcc LD=gcc CPP=/usr/lib/xcpp OBJS_EXT=_80386.o \
	ASMDEF=-DSYSV CFLAGS="$(RSAINCDIR) -Dsco -O -DUNIX -DIDEA32 -DUSE_NBIO"

sco-3.0:
	$(MAKE) all CC=cc LD=cc CPP=/lib/cpp OBJS_EXT=_80386.o \
	ASMDEF=-DSYSV CFLAGS="$(RSAINCDIR) -Dsco -O -DUNIX -DIDEA32 \
	-DUSE_NBIO" LDFLAGS="-lsocket"

sco-3.0-gcc:
	$(MAKE) all CC=gcc LD=gcc CPP=/usr/lib/xcpp OBJS_EXT=_80386.o \
	ASMDEF=-DSYSV CFLAGS="-traditional-cpp $(RSAINCDIR) -Dsco -O \
	-DUNIX -DIDEA32 -DUSE_NBIO" LIBS_EXT=-lsocket

sco386cc:
	$(MAKE) all CPP=/usr/lib/xcpp OBJS_EXT=_80386.o ASMDEF=-DSYSV \
	CFLAGS="$(RSAINCDIR) -Dsco -O -DUNIX -DIDEA32 -DUSE_NBIO -I." \
	LDFLAGS="-lsocket"

# Siemens RM 400 running Sinix 5.42, using the Siemens compiler
sinix:
	$(MAKE) all CPP=/lib/cpp \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_NBIO -DHIGHFIRST"

# Data General Unix using gcc
dgux:
	$(MAKE) all CPP=/lib/cpp CC=gcc LD=gcc\
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_NBIO  \
	-DIDEA32 -DHIGHFIRST"

x286:	xenix286
xenix286:
	$(MAKE) all CC="sh ccc.x286 -M2l" LD="cc -M2l" ASM="cc -M2l" \
	OBJS_EXT=8086.o  LDFLAGS="-F 3000" \
	CFLAGS="$(RSAINCDIR) -LARGE -Ot -DUNIX -DNOPROTO -DSMALL_MEM \
	-DDYN_ALLOC -DUSE_NBIO -DSVR2"

xenix386:
	$(MAKE) all CPP=/lib/cpp OBJS_EXT="_80386.o _zmatch.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DSVR2 -DIDEA32 -DUSE_NBIO -DASM"

3b1:
	$(MAKE) all CC=gcc LD=gcc CPP=/usr/lib/cpp \
	CFLAGS="$(RSAINCDIR) -O -I. -DUNIX -DSVR2 -DPORTABLE -DUSE_NBIO \
	-DHIGHFIRST -DMAX_NAMELEN=14"

3b1_asm:
	$(MAKE) all CC=gcc LD=gcc CPP=/usr/lib/cpp OBJS_EXT=3b168010.o \
	CFLAGS="$(RSAINCDIR) -O -I. -DUNIX -DSVR2 -DUSE_NBIO -DHIGHFIRST \
	-DMAX_NAMELEN=14"

# Silicon Graphics Iris IRIX
sgigcc:
	$(MAKE) all CC=gcc LD=gcc CPP=/usr/lib/cpp \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_NBIO -DHIGHFIRST"

# SGI with assembler modules
sgigcc_asm:
	$(MAKE) all CC=gcc LD=gcc CPP=/usr/lib/cpp OBJS_EXT="r3000.o r3kd.o" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DUSE_NBIO -DHIGHFIRST"

irix:
	$(MAKE) all CC=cc LD=cc \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_NBIO -DHIGHFIRST -acpp"

irix_asm:
	$(MAKE) all CC=cc LD=cc OBJS_EXT="r3000.o r3kd.o" \
	CFLAGS="$(RSAINCDIR) -O -g -DUNIX -DUSE_NBIO -DHIGHFIRST -acpp"

# Users of PGP versions < 2.5 will expect a target 'hpux' to build for
# a snake (HP Series 700 RISC machine).  Don't disappoint them.
hpux:	hpux-pa-ansi

# HP series 700 pa-risc running HP/UX with HP's ANSI cc
hpux-pa-ansi:
	$(MAKE) all CPP=/usr/lib/cpp \
	CFLAGS="$(RSAINCDIR) -Aa +O3 +Obb5000 -D_HPUX_SOURCE -DHIGHFIRST \
	-DUNIX -DPORTABLE -DUSE_SELECT -DIDEA32 -DMERRITT"

# HP series 700/800 pa-risc running HP/UX 9.* with cc
hpux9-pa-ansi:
	$(MAKE) all CPP=/lib/cpp \
	CFLAGS="$(RSAINCDIR) -Ae -D_HPUX_SOURCE +O3 +Onolimit -DHIGHFIRST \
	-DUNIX -DPORTABLE -DUSE_SELECT -DIDEA32 -DMERRITT"


# HP series 700 pa-risc running HP/UX with gcc
hpux-pa-gcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O2 -D_HPUX_SOURCE -DHIGHFIRST -DUNIX \
	-DPORTABLE -DUSE_SELECT -DIDEA32 -DMERRITT"

# HP series 700 pa-risc running HP/UX with HP's standard cc: use unproto
hpux-pa: unproto/cpp
	$(MAKE) all \
	CFLAGS="-tp,./unproto/cpp $(RSAINCDIR) -DHIGHFIRST \
	-DUNIX -DPORTABLE -DUSE_SELECT -DIDEA32 -DMERRITT"

# HP series 300 mc68k running HP/UX with HP's ANSI cc
hpux-68k-ansi:
	$(MAKE) all \
	CFLAGS="$(RSAINCDIR) -Aa +O2 -D_HPUX_SOURCE -DHIGHFIRST -DUNIX \
	-DPORTABLE -DUSE_SELECT -DIDEA32"

# HP series 300 mc68k running HP/UX with gcc
hpux-68k-gcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O2 -D_HPUX_SOURCE -DHIGHFIRST -DUNIX \
	-DPORTABLE -DUSE_SELECT -DIDEA32"

# HP series 300 mc68k running HP/UX with HP's standard cc: use unproto
hpux-68k: unproto/cpp
	$(MAKE) all \
	CFLAGS="-tp,./unproto/cpp $(RSAINCDIR) -DHIGHFIRST \
	-DUNIX -DPORTABLE -DUSE_SELECT -DIDEA32"

# VAX Ultrix 4.2 BSD rev. 96, system #3 with gcc
vax-ultrix:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DPORTABLE -DUSE_SELECT \
	-DSIG_IGN=1 -DSIG_DFL=0"

# DEC Ultrix 4.2 BSD with gcc with MIPSco processors
#
# To use dec's cc on mips you should
# uncomment the next two lines because you can't compile r3000.c 
#r3000.o:	r3000.s
#	$(AS) -o $@ r3000.s
#
#mips-ultrix:
#	$(MAKE) all CC=gcc LD=gcc OBJS_EXT="r3kd.o r3000.o"\
#	CFLAGS="$(RSAINCDIR) $(DBG) -DUNIX -DUSE_SELECT -DIDEA32"

mips-ultrix:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) $(DBG) -DUNIX -DPORTABLE \
	-DMPORTABLE -DUSE_SELECT -DIDEA32"

# Mips R3230 running RISC/os 5.01
riscos501:
	$(MAKE) all CC="cc -systype svr4" LD="cc -systype svr4" \
	CFLAGS="$(RSAINCDIR) -DPORTABLE -DUNIX -DHIGHFIRST -DIDEA32 \
	-DMPORTABLE -DUSE_SELECT -DMERRITT"

convexos:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -DHIGHFIRST -DUNIX \
	-DPORTABLE -DIDEA32"

# RS6000 running AIX
rs6000:
	$(MAKE) all CFLAGS="$(RSAINCDIR) -O -DUNIX -DNOTERMIO -DPORTABLE \
	-DMODERN -DHIGHFIRST -DIDEA32"

# AIX/370
aix370:
	$(MAKE) all CPP=/lib/cpp \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DUSE_NBIO -DSYSV -DHIGHFIRST \
	-DPORTABLE"

# AIX/386 v.1.3
aix386:
	$(MAKE) all CPP="/lib/cpp -DSYSV" \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -DIDEA32 -DUSE_NBIO -DSYSV -DPORTABLE"

next:
	$(MAKE) all \
	CFLAGS="$(RSAINCDIR) -O -arch m68k -DNEXT -DUNIX -DHIGHFIRST \
	-DMACH -DPORTABLE -DNOTERMIO"

next-intel:
	$(MAKE) all OBJS_EXT=_80386.o \
	CFLAGS="-O -DNEXT -DUNIX -DMACH -DNOTERMIO"

next486:
	$(MAKE) all \
	CFLAGS="$(RSAINCDIR) -O -arch i386 -DNEXT -DUNIX -DMACH -DPORTABLE \
	-DIDEA32 -DNOTERMIO"

nextHP:
	$(MAKE) all \
	CFLAGS="$(RSAINCDIR) -O -arch hppa -DNEXT -DUNIX -DMACH -DPORTABLE \
	-DNOTERMIO -DHIGHFIRST"

bsdgcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -I. -DNOTERMIO -D_BSD -DUNIX -DPORTABLE \
	-DUSE_NBIO $(BYTEORDER)"

bsd:
	$(MAKE) all "LD=cc -s" \
	CFLAGS="$(RSAINCDIR) -O -I. -B/lib/ -DNOTERMIO -D_BSD -DUNIX \
	-DPORTABLE -DUSE_NBIO $(BYTEORDER)"

sequent: unproto/cpp
	$(MAKE) all "LD=cc -s -j" CPP=/usr/lib/cpp \
	CFLAGS="$(RSAINCDIR) -j -O -I. -Yp,unproto -DUNIX -DNOTERMIO \
	-DPORTABLE -DUSE_NBIO $(BYTEORDER)"

#  Apollo running DomainOS Release 10.4 (bsd4.3)
apollo:
	$(MAKE) all CC=cc LD=cc \
	CFLAGS="$(RSAINCDIR) -O -I. -DHIGHFIRST -DBSD -DUNIX -DPORTABLE \
	-DUSE_SELECT"

#  Apollo running DomainOS Release 10.4 (bsd4.3) and gcc 2.6.0
apollo-gcc:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -I. -DHIGHFIRST -DBSD -DUNIX -DPORTABLE \
	-DUSE_SELECT"

# RT running bsd with gcc
rt_aos4:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -DUNIX -D_BSD -DMAX_NAMELEN=255 -DPORTABLE \
	-DHIGHFIRST -DNOTERMIO -I."

# Vax running bsd with gcc
vax_bsd43:
	$(MAKE) all CC=gcc LD=gcc \
	CFLAGS="$(RSAINCDIR) -O -I. -DUNIX -D_BSD -DMAX_NAMELEN=255 \
	-DNOTERMIO -DPORTABLE"

# The Open Software Foundation's OSF/1
osf:
	$(MAKE) all \
	CFLAGS="$(RSAINCDIR) $(DBG) -I. -D_BSD -DUNIX -DNOTERMIO \
	-DPORTABLE $(BYTEORDER)"

# A/UX 3.0 using unproto (mine is invoked as a cpp using the CFLAGS shown
# I placed the cpp in a subdirectory of /src/pgp called unproto
aux: unproto/cpp
	$(MAKE) all CC=cc OBJS_EXT=memmove.o \
	LDFLAGS="-lposix -lbsd" \
	CFLAGS="$(RSAINCDIR) -B unproto/ -tp -DSVR2 \
	-DHIGHFIRST -DUNIX -DPORTABLE -DUSE_SELECT -DUPTON"
aux-gcc:
	$(MAKE) all CC=gcc LD=gcc OBJS_EXT=memmove.o \
	LDFLAGS="-lposix -lbsd" \
	CFLAGS="$(RSAINCDIR) -O2 -DSVR2 -DHIGHFIRST -DUNIX -DPORTABLE \
	-DUSE_SELECT -DUPTON"

# Novell UnixWare 2.0 with gcc
unixware:
	$(MAKE) all CC=gcc LD=gcc LDFLAGS="" \
	CFLAGS="$(RSAINCDIR) -O2 -m486 -DUNIX -DIDEA32 -DUSE_NBIO -DPORTABLE"

# Optimized version with 80386.S for OS/2 2.x.
# Requires emx 0.8h (i.e. gcc 2.5.7 or later) and GNU make 3.71 for OS/2
os2:
	$(MAKE) all PROJ=pgp.exe RM=del OBJS_EXT="_80386.o _zmatch.o" \
	CC="gcc -Zomf" CFLAGS="$(RSAINCDIR) -O -DOS2 -DASM -DIDEA32" \
	ASM="gcc -Zomf -c" LD="gcc -Zomf -Zsys" LDFLAGS="-s pgp.def"


# DJGPP, DJ Delorie's port of GNU C to MS-DOS (thanks DJ!).
# (Tested with DJGPP 2.0 and GNU make.)
djgpp:
	$(MAKE) all 'OBJS_EXT=_80386.o _zmatch.o' \
	CC=gcc 'CFLAGS=$(RSAINCDIR) -O3 -DASM -DIDEA32' \
	LD=gcc LIBS_EXT=-lpc RM=del
	strip pgp
	coff2exe pgp

#
# unproto for K&R compilers
#
# unproto was posted on comp.sources.misc: v23i012 v23i013
#
# unpack the unproto package in subdirectory unproto
#

# unproto: needs preprocessed input
unproto/unproto::
	cd unproto && $(MAKE) PROG=unproto PIPE=

# cpp: pipes through /lib/cpp
unproto/cpp::
	cd unproto && $(MAKE)

clean:
	-$(RM) *.o $(PROJ) core a.out tags *.err

tags:
	ctags *.c *.h


## Dependencies ##
armor.o : armor.c mpilib.h usuals.h platform.h fileio.h mpiio.h language.h \
  pgp.h more.h armor.h crypto.h charset.h
charset.o : charset.c usuals.h language.h charset.h system.h
config.o : config.c usuals.h fileio.h pgp.h more.h armor.h config.h \
  charset.h
crypto.o : crypto.c mpilib.h usuals.h platform.h mpiio.h random.h idea.h \
  crypto.h keymgmt.h keymaint.h mdfile.h md5.h fileio.h charset.h language.h \
  pgp.h more.h armor.h exitpgp.h zipup.h rsaglue.h
fileio.o : fileio.c random.h idea.h usuals.h mpilib.h platform.h mpiio.h \
  fileio.h language.h pgp.h more.h armor.h exitpgp.h charset.h system.h
genprime.o : genprime.c mpilib.h usuals.h platform.h genprime.h random.h \
  idea.h
getopt.o : getopt.c getopt.h
idea.o : idea.c idea.h usuals.h
keyadd.o : keyadd.c mpilib.h usuals.h platform.h crypto.h fileio.h \
  keymgmt.h charset.h language.h pgp.h more.h armor.h exitpgp.h keyadd.h \
  keymaint.h
keymaint.o : keymaint.c mpilib.h usuals.h platform.h random.h idea.h \
  crypto.h fileio.h keymgmt.h keymaint.h mpiio.h charset.h language.h pgp.h \
  more.h armor.h
keymgmt.o : keymgmt.c system.h mpilib.h usuals.h platform.h idea.h random.h \
  crypto.h fileio.h keymgmt.h rsagen.h mpiio.h language.h pgp.h more.h \
  armor.h md5.h charset.h keymaint.h
language.o : language.c usuals.h fileio.h language.h pgp.h more.h armor.h \
  charset.h
md5.o : md5.c md5.h
mdfile.o : mdfile.c mpilib.h usuals.h platform.h mdfile.h md5.h fileio.h \
  language.h pgp.h more.h armor.h
more.o : more.c system.h mpilib.h usuals.h platform.h language.h fileio.h \
  pgp.h more.h armor.h charset.h
mpiio.o : mpiio.c mpilib.h # usuals.h platform.h mpiio.h pgp.h more.h armor.h
mpilib.o : mpilib.c mpilib.h # usuals.h platform.h
passwd.o : passwd.c random.h idea.h usuals.h md5.h language.h pgp.h more.h \
  armor.h
pgp.o : pgp.c system.h mpilib.h usuals.h platform.h random.h idea.h crypto.h \
  fileio.h keymgmt.h language.h pgp.h more.h armor.h exitpgp.h charset.h \
  getopt.h config.h keymaint.h keyadd.h rsaglue.h
random.o : random.c system.h random.h idea.h usuals.h language.h
rsagen.o : rsagen.c mpilib.h usuals.h platform.h genprime.h rsagen.h \
  random.h idea.h rsaglue.h
rsaglue.o : rsaglue.c mpilib.h mpiio.h pgp.h rsaglue.h
system.o : system.c exitpgp.h system.h charset.h mpilib.h
zbits.o : zbits.c zip.h ztailor.h ziperr.h
zdeflate.o : zdeflate.c zunzip.h usuals.h system.h zip.h ztailor.h ziperr.h
zfile_io.o : zfile_io.c zunzip.h usuals.h system.h
zglobals.o : zglobals.c zip.h ztailor.h ziperr.h
zinflate.o : zinflate.c zunzip.h usuals.h system.h exitpgp.h
zip.o : zip.c zip.h usuals.h fileio.h language.h pgp.h more.h armor.h exitpgp.h
zipup.o : zipup.c zip.h ztailor.h ziperr.h zrevisio.h
ztrees.o : ztrees.c zip.h ztailor.h ziperr.h
zunzip.o : zunzip.c zunzip.h usuals.h system.h language.h
r3000.o : r3000.c mpilib.h usuals.h platform.h lmul.h

# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=pgpwin32 - Win32 Portable Debug
!MESSAGE No configuration specified.  Defaulting to pgpwin32 - Win32 Portable\
 Debug.
!ENDIF 

!IF "$(CFG)" != "pgpwin32 - Win32 Portable Release" && "$(CFG)" !=\
 "pgpwin32 - Win32 Portable Debug" && "$(CFG)" !=\
 "pgpwin32 - Win32 Intel Release" && "$(CFG)" != "pgpwin32 - Win32 Intel Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "pgpwin40.mak" CFG="pgpwin32 - Win32 Portable Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pgpwin32 - Win32 Portable Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "pgpwin32 - Win32 Portable Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "pgpwin32 - Win32 Intel Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "pgpwin32 - Win32 Intel Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\pgp.exe"

CLEAN : 
	-@erase ".\WinRel\pgp.exe"
	-@erase ".\WinRel\zglobals.obj"
	-@erase ".\WinRel\random.obj"
	-@erase ".\WinRel\crypto.obj"
	-@erase ".\WinRel\idea.obj"
	-@erase ".\WinRel\zunzip.obj"
	-@erase ".\WinRel\randpool.obj"
	-@erase ".\WinRel\passwd.obj"
	-@erase ".\WinRel\zipup.obj"
	-@erase ".\WinRel\md5.obj"
	-@erase ".\WinRel\mpiio.obj"
	-@erase ".\WinRel\mpilib.obj"
	-@erase ".\WinRel\rsagen.obj"
	-@erase ".\WinRel\genprime.obj"
	-@erase ".\WinRel\zfile_io.obj"
	-@erase ".\WinRel\strtol.obj"
	-@erase ".\WinRel\zip.obj"
	-@erase ".\WinRel\keymaint.obj"
	-@erase ".\WinRel\config.obj"
	-@erase ".\WinRel\rsaglue1.obj"
	-@erase ".\WinRel\mdfile.obj"
	-@erase ".\WinRel\memmove.obj"
	-@erase ".\WinRel\zbits.obj"
	-@erase ".\WinRel\sleep.obj"
	-@erase ".\WinRel\keyadd.obj"
	-@erase ".\WinRel\armor.obj"
	-@erase ".\WinRel\fileio.obj"
	-@erase ".\WinRel\noise.obj"
	-@erase ".\WinRel\keymgmt.obj"
	-@erase ".\WinRel\system.obj"
	-@erase ".\WinRel\ztrees.obj"
	-@erase ".\WinRel\getopt.obj"
	-@erase ".\WinRel\zdeflate.obj"
	-@erase ".\WinRel\charset.obj"
	-@erase ".\WinRel\more.obj"
	-@erase ".\WinRel\language.obj"
	-@erase ".\WinRel\zinflate.obj"
	-@erase ".\WinRel\pgp.obj"
	-@erase ".\WinRel\mpw32asm.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

$(OUTDIR) : 

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)

$(OUTDIR)/"pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)

.c{$(CPP_OBJS)}.obj:

.cpp{$(CPP_OBJS)}.obj:

.cxx{$(CPP_OBJS)}.obj:

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/pgpwin40.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pgpwin40.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"WinRel/pgp.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/pgp.pdb" /machine:I386 /out:"$(OUTDIR)/pgp.exe" 
LINK32_OBJS= \
	"$(INTDIR)/zglobals.obj" \
	"$(INTDIR)/random.obj" \
	"$(INTDIR)/crypto.obj" \
	"$(INTDIR)/idea.obj" \
	"$(INTDIR)/zunzip.obj" \
	"$(INTDIR)/randpool.obj" \
	"$(INTDIR)/passwd.obj" \
	"$(INTDIR)/zipup.obj" \
	"$(INTDIR)/md5.obj" \
	"$(INTDIR)/mpiio.obj" \
	"$(INTDIR)/mpilib.obj" \
	"$(INTDIR)/rsagen.obj" \
	"$(INTDIR)/genprime.obj" \
	"$(INTDIR)/zfile_io.obj" \
	"$(INTDIR)/strtol.obj" \
	"$(INTDIR)/zip.obj" \
	"$(INTDIR)/keymaint.obj" \
	"$(INTDIR)/config.obj" \
	"$(INTDIR)/rsaglue1.obj" \
	"$(INTDIR)/mdfile.obj" \
	"$(INTDIR)/memmove.obj" \
	"$(INTDIR)/zbits.obj" \
	"$(INTDIR)/sleep.obj" \
	"$(INTDIR)/keyadd.obj" \
	"$(INTDIR)/armor.obj" \
	"$(INTDIR)/fileio.obj" \
	"$(INTDIR)/noise.obj" \
	"$(INTDIR)/keymgmt.obj" \
	"$(INTDIR)/system.obj" \
	"$(INTDIR)/ztrees.obj" \
	"$(INTDIR)/getopt.obj" \
	"$(INTDIR)/zdeflate.obj" \
	"$(INTDIR)/charset.obj" \
	"$(INTDIR)/more.obj" \
	"$(INTDIR)/language.obj" \
	"$(INTDIR)/zinflate.obj" \
	"$(INTDIR)/pgp.obj" \
	"$(INTDIR)/mpw32asm.obj"

"$(OUTDIR)\pgp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\pgp.exe"

CLEAN : 
	-@erase ".\WinDebug\pgp.exe"
	-@erase ".\WinDebug\zglobals.obj"
	-@erase ".\WinDebug\random.obj"
	-@erase ".\WinDebug\crypto.obj"
	-@erase ".\WinDebug\idea.obj"
	-@erase ".\WinDebug\zunzip.obj"
	-@erase ".\WinDebug\randpool.obj"
	-@erase ".\WinDebug\passwd.obj"
	-@erase ".\WinDebug\zipup.obj"
	-@erase ".\WinDebug\md5.obj"
	-@erase ".\WinDebug\mpiio.obj"
	-@erase ".\WinDebug\mpilib.obj"
	-@erase ".\WinDebug\rsagen.obj"
	-@erase ".\WinDebug\genprime.obj"
	-@erase ".\WinDebug\zfile_io.obj"
	-@erase ".\WinDebug\strtol.obj"
	-@erase ".\WinDebug\zip.obj"
	-@erase ".\WinDebug\keymaint.obj"
	-@erase ".\WinDebug\config.obj"
	-@erase ".\WinDebug\rsaglue1.obj"
	-@erase ".\WinDebug\mdfile.obj"
	-@erase ".\WinDebug\memmove.obj"
	-@erase ".\WinDebug\zbits.obj"
	-@erase ".\WinDebug\sleep.obj"
	-@erase ".\WinDebug\keyadd.obj"
	-@erase ".\WinDebug\armor.obj"
	-@erase ".\WinDebug\fileio.obj"
	-@erase ".\WinDebug\noise.obj"
	-@erase ".\WinDebug\keymgmt.obj"
	-@erase ".\WinDebug\system.obj"
	-@erase ".\WinDebug\ztrees.obj"
	-@erase ".\WinDebug\getopt.obj"
	-@erase ".\WinDebug\zdeflate.obj"
	-@erase ".\WinDebug\charset.obj"
	-@erase ".\WinDebug\more.obj"
	-@erase ".\WinDebug\language.obj"
	-@erase ".\WinDebug\zinflate.obj"
	-@erase ".\WinDebug\pgp.obj"
	-@erase ".\WinDebug\mpw32asm.obj"
	-@erase ".\WinDebug\pgp.ilk"
	-@erase ".\WinDebug\pgp.pdb"
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

$(OUTDIR) : 

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)

$(OUTDIR)/"pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)

.c{$(CPP_OBJS)}.obj:

.cpp{$(CPP_OBJS)}.obj:

.cxx{$(CPP_OBJS)}.obj:

# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/pgpwin40.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pgpwin40.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"WinDebug/pgp.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/pgp.pdb" /debug /machine:I386 /out:"$(OUTDIR)/pgp.exe" 
LINK32_OBJS= \
	"$(INTDIR)/zglobals.obj" \
	"$(INTDIR)/random.obj" \
	"$(INTDIR)/crypto.obj" \
	"$(INTDIR)/idea.obj" \
	"$(INTDIR)/zunzip.obj" \
	"$(INTDIR)/randpool.obj" \
	"$(INTDIR)/passwd.obj" \
	"$(INTDIR)/zipup.obj" \
	"$(INTDIR)/md5.obj" \
	"$(INTDIR)/mpiio.obj" \
	"$(INTDIR)/mpilib.obj" \
	"$(INTDIR)/rsagen.obj" \
	"$(INTDIR)/genprime.obj" \
	"$(INTDIR)/zfile_io.obj" \
	"$(INTDIR)/strtol.obj" \
	"$(INTDIR)/zip.obj" \
	"$(INTDIR)/keymaint.obj" \
	"$(INTDIR)/config.obj" \
	"$(INTDIR)/rsaglue1.obj" \
	"$(INTDIR)/mdfile.obj" \
	"$(INTDIR)/memmove.obj" \
	"$(INTDIR)/zbits.obj" \
	"$(INTDIR)/sleep.obj" \
	"$(INTDIR)/keyadd.obj" \
	"$(INTDIR)/armor.obj" \
	"$(INTDIR)/fileio.obj" \
	"$(INTDIR)/noise.obj" \
	"$(INTDIR)/keymgmt.obj" \
	"$(INTDIR)/system.obj" \
	"$(INTDIR)/ztrees.obj" \
	"$(INTDIR)/getopt.obj" \
	"$(INTDIR)/zdeflate.obj" \
	"$(INTDIR)/charset.obj" \
	"$(INTDIR)/more.obj" \
	"$(INTDIR)/language.obj" \
	"$(INTDIR)/zinflate.obj" \
	"$(INTDIR)/pgp.obj" \
	"$(INTDIR)/mpw32asm.obj"

"$(OUTDIR)\pgp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PGP_Rele"
# PROP BASE Intermediate_Dir "PGP_Rele"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Intel_Release"
# PROP Intermediate_Dir "Intel_Release"
OUTDIR=.\Intel_Release
INTDIR=.\Intel_Release

ALL : "$(OUTDIR)\pgp.exe"

CLEAN : 
	-@erase ".\Intel_Release\pgp.exe"
	-@erase ".\Intel_Release\zglobals.obj"
	-@erase ".\Intel_Release\random.obj"
	-@erase ".\Intel_Release\crypto.obj"
	-@erase ".\Intel_Release\idea.obj"
	-@erase ".\Intel_Release\zunzip.obj"
	-@erase ".\Intel_Release\randpool.obj"
	-@erase ".\Intel_Release\passwd.obj"
	-@erase ".\Intel_Release\zipup.obj"
	-@erase ".\Intel_Release\md5.obj"
	-@erase ".\Intel_Release\mpiio.obj"
	-@erase ".\Intel_Release\mpilib.obj"
	-@erase ".\Intel_Release\rsagen.obj"
	-@erase ".\Intel_Release\genprime.obj"
	-@erase ".\Intel_Release\zfile_io.obj"
	-@erase ".\Intel_Release\strtol.obj"
	-@erase ".\Intel_Release\zip.obj"
	-@erase ".\Intel_Release\keymaint.obj"
	-@erase ".\Intel_Release\config.obj"
	-@erase ".\Intel_Release\rsaglue1.obj"
	-@erase ".\Intel_Release\mdfile.obj"
	-@erase ".\Intel_Release\memmove.obj"
	-@erase ".\Intel_Release\zbits.obj"
	-@erase ".\Intel_Release\sleep.obj"
	-@erase ".\Intel_Release\keyadd.obj"
	-@erase ".\Intel_Release\armor.obj"
	-@erase ".\Intel_Release\fileio.obj"
	-@erase ".\Intel_Release\noise.obj"
	-@erase ".\Intel_Release\keymgmt.obj"
	-@erase ".\Intel_Release\system.obj"
	-@erase ".\Intel_Release\ztrees.obj"
	-@erase ".\Intel_Release\getopt.obj"
	-@erase ".\Intel_Release\zdeflate.obj"
	-@erase ".\Intel_Release\charset.obj"
	-@erase ".\Intel_Release\more.obj"
	-@erase ".\Intel_Release\language.obj"
	-@erase ".\Intel_Release\zinflate.obj"
	-@erase ".\Intel_Release\pgp.obj"
	-@erase ".\Intel_Release\mpw32asm.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

$(OUTDIR) : 

$(INTDIR) : 

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)

".\Intel_Release\pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)

.c{$(CPP_OBJS)}.obj:

.cpp{$(CPP_OBJS)}.obj:

.cxx{$(CPP_OBJS)}.obj:

# ADD BASE CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "USE_WIN32_ASSEMBLER" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "USE_WIN32_ASSEMBLER" /Fp"$(INTDIR)/pgpwin40.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Intel_Release/
CPP_SBRS=
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pgpwin40.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"WinRel/pgp.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Intel_Release\pgp.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/pgp.pdb" /machine:I386 /out:"$(OUTDIR)/pgp.exe" 
LINK32_OBJS= \
	"$(INTDIR)/zglobals.obj" \
	"$(INTDIR)/random.obj" \
	"$(INTDIR)/crypto.obj" \
	"$(INTDIR)/idea.obj" \
	"$(INTDIR)/zunzip.obj" \
	"$(INTDIR)/randpool.obj" \
	"$(INTDIR)/passwd.obj" \
	"$(INTDIR)/zipup.obj" \
	"$(INTDIR)/md5.obj" \
	"$(INTDIR)/mpiio.obj" \
	"$(INTDIR)/mpilib.obj" \
	"$(INTDIR)/rsagen.obj" \
	"$(INTDIR)/genprime.obj" \
	"$(INTDIR)/zfile_io.obj" \
	"$(INTDIR)/strtol.obj" \
	"$(INTDIR)/zip.obj" \
	"$(INTDIR)/keymaint.obj" \
	"$(INTDIR)/config.obj" \
	"$(INTDIR)/rsaglue1.obj" \
	"$(INTDIR)/mdfile.obj" \
	"$(INTDIR)/memmove.obj" \
	"$(INTDIR)/zbits.obj" \
	"$(INTDIR)/sleep.obj" \
	"$(INTDIR)/keyadd.obj" \
	"$(INTDIR)/armor.obj" \
	"$(INTDIR)/fileio.obj" \
	"$(INTDIR)/noise.obj" \
	"$(INTDIR)/keymgmt.obj" \
	"$(INTDIR)/system.obj" \
	"$(INTDIR)/ztrees.obj" \
	"$(INTDIR)/getopt.obj" \
	"$(INTDIR)/zdeflate.obj" \
	"$(INTDIR)/charset.obj" \
	"$(INTDIR)/more.obj" \
	"$(INTDIR)/language.obj" \
	"$(INTDIR)/zinflate.obj" \
	"$(INTDIR)/pgp.obj" \
	"$(INTDIR)/mpw32asm.obj"

"$(OUTDIR)\pgp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Intel_"
# PROP BASE Intermediate_Dir "Intel_"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Intel_Debug"
# PROP Intermediate_Dir "Intel_Debug"
OUTDIR=.\Intel_Debug
INTDIR=.\Intel_Debug

ALL : "$(OUTDIR)\pgp.exe"

CLEAN : 
	-@erase ".\Intel_Debug\pgp.exe"
	-@erase ".\Intel_Debug\zglobals.obj"
	-@erase ".\Intel_Debug\random.obj"
	-@erase ".\Intel_Debug\crypto.obj"
	-@erase ".\Intel_Debug\idea.obj"
	-@erase ".\Intel_Debug\zunzip.obj"
	-@erase ".\Intel_Debug\randpool.obj"
	-@erase ".\Intel_Debug\passwd.obj"
	-@erase ".\Intel_Debug\zipup.obj"
	-@erase ".\Intel_Debug\md5.obj"
	-@erase ".\Intel_Debug\mpiio.obj"
	-@erase ".\Intel_Debug\mpilib.obj"
	-@erase ".\Intel_Debug\rsagen.obj"
	-@erase ".\Intel_Debug\genprime.obj"
	-@erase ".\Intel_Debug\zfile_io.obj"
	-@erase ".\Intel_Debug\strtol.obj"
	-@erase ".\Intel_Debug\zip.obj"
	-@erase ".\Intel_Debug\keymaint.obj"
	-@erase ".\Intel_Debug\config.obj"
	-@erase ".\Intel_Debug\rsaglue1.obj"
	-@erase ".\Intel_Debug\mdfile.obj"
	-@erase ".\Intel_Debug\memmove.obj"
	-@erase ".\Intel_Debug\zbits.obj"
	-@erase ".\Intel_Debug\sleep.obj"
	-@erase ".\Intel_Debug\keyadd.obj"
	-@erase ".\Intel_Debug\armor.obj"
	-@erase ".\Intel_Debug\fileio.obj"
	-@erase ".\Intel_Debug\noise.obj"
	-@erase ".\Intel_Debug\keymgmt.obj"
	-@erase ".\Intel_Debug\system.obj"
	-@erase ".\Intel_Debug\ztrees.obj"
	-@erase ".\Intel_Debug\getopt.obj"
	-@erase ".\Intel_Debug\zdeflate.obj"
	-@erase ".\Intel_Debug\charset.obj"
	-@erase ".\Intel_Debug\more.obj"
	-@erase ".\Intel_Debug\language.obj"
	-@erase ".\Intel_Debug\zinflate.obj"
	-@erase ".\Intel_Debug\pgp.obj"
	-@erase ".\Intel_Debug\mpw32asm.obj"
	-@erase ".\Intel_Debug\pgp.ilk"
	-@erase ".\Intel_Debug\pgp.pdb"
	-@erase ".\Intel_Debug\pgpwin32.pdb"
	-@erase ".\Intel_Debug\pgpwin32.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

$(OUTDIR) : 

$(INTDIR) : 

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)

".\Intel_Debug\pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)

.c{$(CPP_OBJS)}.obj:

.cpp{$(CPP_OBJS)}.obj:

.cxx{$(CPP_OBJS)}.obj:

# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "USE_WIN32_ASSEMBLER" /YX /Fd"Intel_Debug/pgpwin32.pdb" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /D "USE_WIN32_ASSEMBLER" /Fp"$(INTDIR)/pgpwin40.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/pgpwin32.pdb" /c 
CPP_OBJS=.\Intel_Debug/
CPP_SBRS=
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/pgpwin40.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"WinDebug/pgp.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Intel_Debug/pgp.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/pgp.pdb" /debug /machine:I386 /out:"$(OUTDIR)/pgp.exe" 
LINK32_OBJS= \
	"$(INTDIR)/zglobals.obj" \
	"$(INTDIR)/random.obj" \
	"$(INTDIR)/crypto.obj" \
	"$(INTDIR)/idea.obj" \
	"$(INTDIR)/zunzip.obj" \
	"$(INTDIR)/randpool.obj" \
	"$(INTDIR)/passwd.obj" \
	"$(INTDIR)/zipup.obj" \
	"$(INTDIR)/md5.obj" \
	"$(INTDIR)/mpiio.obj" \
	"$(INTDIR)/mpilib.obj" \
	"$(INTDIR)/rsagen.obj" \
	"$(INTDIR)/genprime.obj" \
	"$(INTDIR)/zfile_io.obj" \
	"$(INTDIR)/strtol.obj" \
	"$(INTDIR)/zip.obj" \
	"$(INTDIR)/keymaint.obj" \
	"$(INTDIR)/config.obj" \
	"$(INTDIR)/rsaglue1.obj" \
	"$(INTDIR)/mdfile.obj" \
	"$(INTDIR)/memmove.obj" \
	"$(INTDIR)/zbits.obj" \
	"$(INTDIR)/sleep.obj" \
	"$(INTDIR)/keyadd.obj" \
	"$(INTDIR)/armor.obj" \
	"$(INTDIR)/fileio.obj" \
	"$(INTDIR)/noise.obj" \
	"$(INTDIR)/keymgmt.obj" \
	"$(INTDIR)/system.obj" \
	"$(INTDIR)/ztrees.obj" \
	"$(INTDIR)/getopt.obj" \
	"$(INTDIR)/zdeflate.obj" \
	"$(INTDIR)/charset.obj" \
	"$(INTDIR)/more.obj" \
	"$(INTDIR)/language.obj" \
	"$(INTDIR)/zinflate.obj" \
	"$(INTDIR)/pgp.obj" \
	"$(INTDIR)/mpw32asm.obj"

"$(OUTDIR)\pgp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "pgpwin32 - Win32 Portable Release"
# Name "pgpwin32 - Win32 Portable Debug"
# Name "pgpwin32 - Win32 Intel Release"
# Name "pgpwin32 - Win32 Intel Debug"

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"

!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"

!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"

!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\zglobals.c
DEP_CPP_ZGLOB=\
	".\zip.h"\
	".\ztailor.h"\
	".\ziperr.h"\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zglobals.obj" : $(SOURCE) $(DEP_CPP_ZGLOB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zglobals.obj" : $(SOURCE) $(DEP_CPP_ZGLOB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zglobals.obj" : $(SOURCE) $(DEP_CPP_ZGLOB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zglobals.obj" : $(SOURCE) $(DEP_CPP_ZGLOB) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\random.c
DEP_CPP_RANDO=\
	{$(INCLUDE)}"\stdlib.h"\
	".\system.h"\
	".\idea.h"\
	".\md5.h"\
	".\noise.h"\
	".\language.h"\
	".\random.h"\
	".\fileio.h"\
	".\pgp.h"\
	".\randpool.h"\
	{$(INCLUDE)}"\sys\types.h"\
	".\usuals.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_RANDO=\
	".\Macutil2.h"\
	".\Macutil3.h"\
	".\TimeManager.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\random.obj" : $(SOURCE) $(DEP_CPP_RANDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\random.obj" : $(SOURCE) $(DEP_CPP_RANDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\random.obj" : $(SOURCE) $(DEP_CPP_RANDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\random.obj" : $(SOURCE) $(DEP_CPP_RANDO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crypto.c
DEP_CPP_CRYPT=\
	{$(INCLUDE)}"\stdlib.h"\
	".\mpilib.h"\
	".\mpiio.h"\
	".\random.h"\
	".\crypto.h"\
	".\keymgmt.h"\
	".\keymaint.h"\
	".\mdfile.h"\
	".\fileio.h"\
	".\charset.h"\
	".\language.h"\
	".\pgp.h"\
	".\exitpgp.h"\
	".\zipup.h"\
	".\rsaglue.h"\
	".\idea.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\md5.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_CRYPT=\
	".\Macutil.h"\
	".\Macutil2.h"\
	".\Macutil3.h"\
	".\Aestuff.h"\
	".\MyBufferedStdio.h"\
	".\ReplaceStdio.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\crypto.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\crypto.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\crypto.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\crypto.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\idea.c
DEP_CPP_IDEA_=\
	".\idea.h"\
	".\randpool.h"\
	".\usuals.h"\
	".\c370.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\idea.obj" : $(SOURCE) $(DEP_CPP_IDEA_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\idea.obj" : $(SOURCE) $(DEP_CPP_IDEA_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\idea.obj" : $(SOURCE) $(DEP_CPP_IDEA_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\idea.obj" : $(SOURCE) $(DEP_CPP_IDEA_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zunzip.c
DEP_CPP_ZUNZI=\
	".\zunzip.h"\
	".\language.h"\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\system.h"\
	{$(INCLUDE)}"\sys\timeb.h"\
	{$(INCLUDE)}"\os2.h"\
	{$(INCLUDE)}"\file.h"\
	".\c370.h"\
	{$(INCLUDE)}"\sys\types.h"\
	"S:\common\inc\btosdef.h"\
	"S:\common\inc\btdos.h"\
	"S:\common\inc\BTSTRUCT.H"\
	"S:\common\inc\vpragma.h"\
	
NODEP_CPP_ZUNZI=\
	".\macstat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zunzip.obj" : $(SOURCE) $(DEP_CPP_ZUNZI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zunzip.obj" : $(SOURCE) $(DEP_CPP_ZUNZI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zunzip.obj" : $(SOURCE) $(DEP_CPP_ZUNZI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zunzip.obj" : $(SOURCE) $(DEP_CPP_ZUNZI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\randpool.c
DEP_CPP_RANDP=\
	{$(INCLUDE)}"\stdlib.h"\
	".\randpool.h"\
	".\usuals.h"\
	".\md5.h"\
	".\c370.h"\
	
NODEP_CPP_RANDP=\
	".\TimeManager.h"\
	".\Random pool too small - please increase RANDPOOLBITS in randpool.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\randpool.obj" : $(SOURCE) $(DEP_CPP_RANDP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\randpool.obj" : $(SOURCE) $(DEP_CPP_RANDP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\randpool.obj" : $(SOURCE) $(DEP_CPP_RANDP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\randpool.obj" : $(SOURCE) $(DEP_CPP_RANDP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\passwd.c
DEP_CPP_PASSW=\
	".\random.h"\
	".\md5.h"\
	".\language.h"\
	".\pgp.h"\
	".\charset.h"\
	".\system.h"\
	".\usuals.h"\
	".\idea.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\stdlib.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\passwd.obj" : $(SOURCE) $(DEP_CPP_PASSW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\passwd.obj" : $(SOURCE) $(DEP_CPP_PASSW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\passwd.obj" : $(SOURCE) $(DEP_CPP_PASSW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\passwd.obj" : $(SOURCE) $(DEP_CPP_PASSW) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zipup.c
DEP_CPP_ZIPUP=\
	".\zip.h"\
	".\zrevisio.h"\
	".\system.h"\
	".\ztailor.h"\
	".\ziperr.h"\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zipup.obj" : $(SOURCE) $(DEP_CPP_ZIPUP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zipup.obj" : $(SOURCE) $(DEP_CPP_ZIPUP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zipup.obj" : $(SOURCE) $(DEP_CPP_ZIPUP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zipup.obj" : $(SOURCE) $(DEP_CPP_ZIPUP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\md5.c
DEP_CPP_MD5_C=\
	".\md5.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpiio.c
DEP_CPP_MPIIO=\
	".\mpilib.h"\
	".\mpiio.h"\
	".\pgp.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\mpiio.obj" : $(SOURCE) $(DEP_CPP_MPIIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\mpiio.obj" : $(SOURCE) $(DEP_CPP_MPIIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\mpiio.obj" : $(SOURCE) $(DEP_CPP_MPIIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\mpiio.obj" : $(SOURCE) $(DEP_CPP_MPIIO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpilib.c
DEP_CPP_MPILI=\
	".\mpilib.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	
NODEP_CPP_MPILI=\
	".\Macutil3.h"\
	".\UNITSIZE cannot be smaller than MULTUNITSIZE"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\mpilib.obj" : $(SOURCE) $(DEP_CPP_MPILI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\mpilib.obj" : $(SOURCE) $(DEP_CPP_MPILI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\mpilib.obj" : $(SOURCE) $(DEP_CPP_MPILI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\mpilib.obj" : $(SOURCE) $(DEP_CPP_MPILI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rsagen.c
DEP_CPP_RSAGE=\
	".\mpilib.h"\
	".\genprime.h"\
	".\rsagen.h"\
	".\random.h"\
	".\rsaglue.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\idea.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\rsagen.obj" : $(SOURCE) $(DEP_CPP_RSAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\rsagen.obj" : $(SOURCE) $(DEP_CPP_RSAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\rsagen.obj" : $(SOURCE) $(DEP_CPP_RSAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\rsagen.obj" : $(SOURCE) $(DEP_CPP_RSAGE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\genprime.c
DEP_CPP_GENPR=\
	{$(INCLUDE)}"\stdlib.h"\
	".\mpilib.h"\
	".\genprime.h"\
	".\random.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\idea.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\genprime.obj" : $(SOURCE) $(DEP_CPP_GENPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\genprime.obj" : $(SOURCE) $(DEP_CPP_GENPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\genprime.obj" : $(SOURCE) $(DEP_CPP_GENPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\genprime.obj" : $(SOURCE) $(DEP_CPP_GENPR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zfile_io.c
DEP_CPP_ZFILE=\
	".\zunzip.h"\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\system.h"\
	{$(INCLUDE)}"\sys\timeb.h"\
	{$(INCLUDE)}"\os2.h"\
	{$(INCLUDE)}"\file.h"\
	".\c370.h"\
	{$(INCLUDE)}"\sys\types.h"\
	"S:\common\inc\btosdef.h"\
	"S:\common\inc\btdos.h"\
	"S:\common\inc\BTSTRUCT.H"\
	"S:\common\inc\vpragma.h"\
	
NODEP_CPP_ZFILE=\
	".\macstat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zfile_io.obj" : $(SOURCE) $(DEP_CPP_ZFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zfile_io.obj" : $(SOURCE) $(DEP_CPP_ZFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zfile_io.obj" : $(SOURCE) $(DEP_CPP_ZFILE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zfile_io.obj" : $(SOURCE) $(DEP_CPP_ZFILE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strtol.c
DEP_CPP_STRTO=\
	{$(INCLUDE)}"\stdlib.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\strtol.obj" : $(SOURCE) $(DEP_CPP_STRTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\strtol.obj" : $(SOURCE) $(DEP_CPP_STRTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\strtol.obj" : $(SOURCE) $(DEP_CPP_STRTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\strtol.obj" : $(SOURCE) $(DEP_CPP_STRTO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zip.c
DEP_CPP_ZIP_C=\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\fileio.h"\
	".\language.h"\
	".\pgp.h"\
	".\exitpgp.h"\
	".\zip.h"\
	".\ziperr.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	".\ztailor.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zip.obj" : $(SOURCE) $(DEP_CPP_ZIP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zip.obj" : $(SOURCE) $(DEP_CPP_ZIP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zip.obj" : $(SOURCE) $(DEP_CPP_ZIP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zip.obj" : $(SOURCE) $(DEP_CPP_ZIP_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keymaint.c
DEP_CPP_KEYMA=\
	{$(INCLUDE)}"\stdlib.h"\
	".\mpilib.h"\
	".\random.h"\
	".\crypto.h"\
	".\fileio.h"\
	".\keymgmt.h"\
	".\keymaint.h"\
	".\mpiio.h"\
	".\charset.h"\
	".\language.h"\
	".\pgp.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\idea.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_KEYMA=\
	".\Macutil3.h"\
	".\PGPDialogs.h"\
	".\MyBufferedStdio.h"\
	".\ReplaceStdio.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\keymaint.obj" : $(SOURCE) $(DEP_CPP_KEYMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\keymaint.obj" : $(SOURCE) $(DEP_CPP_KEYMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\keymaint.obj" : $(SOURCE) $(DEP_CPP_KEYMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\keymaint.obj" : $(SOURCE) $(DEP_CPP_KEYMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\config.c
DEP_CPP_CONFI=\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\fileio.h"\
	".\language.h"\
	".\pgp.h"\
	".\config.h"\
	".\charset.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rsaglue1.c
DEP_CPP_RSAGL=\
	".\mpilib.h"\
	".\mpiio.h"\
	".\pgp.h"\
	".\rsaglue.h"\
	".\random.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	".\idea.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\rsaglue1.obj" : $(SOURCE) $(DEP_CPP_RSAGL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\rsaglue1.obj" : $(SOURCE) $(DEP_CPP_RSAGL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\rsaglue1.obj" : $(SOURCE) $(DEP_CPP_RSAGL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\rsaglue1.obj" : $(SOURCE) $(DEP_CPP_RSAGL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdfile.c
DEP_CPP_MDFIL=\
	".\mpilib.h"\
	".\mdfile.h"\
	".\fileio.h"\
	".\language.h"\
	".\pgp.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\md5.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_MDFIL=\
	".\Macutil3.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\mdfile.obj" : $(SOURCE) $(DEP_CPP_MDFIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\mdfile.obj" : $(SOURCE) $(DEP_CPP_MDFIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\mdfile.obj" : $(SOURCE) $(DEP_CPP_MDFIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\mdfile.obj" : $(SOURCE) $(DEP_CPP_MDFIL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memmove.c
DEP_CPP_MEMMO=\
	{$(INCLUDE)}"\sys\types.h"\
	".\cdefs.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\memmove.obj" : $(SOURCE) $(DEP_CPP_MEMMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\memmove.obj" : $(SOURCE) $(DEP_CPP_MEMMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\memmove.obj" : $(SOURCE) $(DEP_CPP_MEMMO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\memmove.obj" : $(SOURCE) $(DEP_CPP_MEMMO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zbits.c
DEP_CPP_ZBITS=\
	".\zip.h"\
	".\ztailor.h"\
	".\ziperr.h"\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zbits.obj" : $(SOURCE) $(DEP_CPP_ZBITS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zbits.obj" : $(SOURCE) $(DEP_CPP_ZBITS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zbits.obj" : $(SOURCE) $(DEP_CPP_ZBITS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zbits.obj" : $(SOURCE) $(DEP_CPP_ZBITS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sleep.c

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\sleep.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\sleep.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\sleep.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\sleep.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keyadd.c
DEP_CPP_KEYAD=\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	".\mpilib.h"\
	".\crypto.h"\
	".\fileio.h"\
	".\keymgmt.h"\
	".\charset.h"\
	".\mpiio.h"\
	".\language.h"\
	".\pgp.h"\
	".\exitpgp.h"\
	".\keyadd.h"\
	".\keymaint.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\idea.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_KEYAD=\
	".\Macutil2.h"\
	".\Macutil3.h"\
	".\MyBufferedStdio.h"\
	".\ReplaceStdio.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\keyadd.obj" : $(SOURCE) $(DEP_CPP_KEYAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\keyadd.obj" : $(SOURCE) $(DEP_CPP_KEYAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\keyadd.obj" : $(SOURCE) $(DEP_CPP_KEYAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\keyadd.obj" : $(SOURCE) $(DEP_CPP_KEYAD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\armor.c
DEP_CPP_ARMOR=\
	".\mpilib.h"\
	".\fileio.h"\
	".\mpiio.h"\
	".\language.h"\
	".\pgp.h"\
	".\charset.h"\
	".\crypto.h"\
	".\armor.h"\
	".\keymgmt.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\more.h"\
	".\idea.h"\
	
NODEP_CPP_ARMOR=\
	".\Macutil2.h"\
	".\Macutil3.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\armor.obj" : $(SOURCE) $(DEP_CPP_ARMOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\armor.obj" : $(SOURCE) $(DEP_CPP_ARMOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\armor.obj" : $(SOURCE) $(DEP_CPP_ARMOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\armor.obj" : $(SOURCE) $(DEP_CPP_ARMOR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fileio.c
DEP_CPP_FILEI=\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\file.h"\
	".\random.h"\
	".\mpilib.h"\
	".\mpiio.h"\
	".\fileio.h"\
	".\language.h"\
	".\pgp.h"\
	".\exitpgp.h"\
	".\charset.h"\
	".\system.h"\
	".\crypto.h"\
	"S:\common\inc\vpragma.h"\
	".\usuals.h"\
	".\idea.h"\
	".\c370.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_FILEI=\
	".\Aestuff.h"\
	".\AppGlobals.h"\
	".\MacPGP.h"\
	".\Macutil2.h"\
	".\Macutil3.h"\
	".\Unknown OS"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\fileio.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\noise.c
DEP_CPP_NOISE=\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\randpool.h"\
	".\noise.h"\
	".\pgp.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_NOISE=\
	".\TimeManager.h"\
	".\This compiler is not supported, modify the code above accordingly"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\noise.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\noise.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\noise.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\noise.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keymgmt.c
DEP_CPP_KEYMG=\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	".\system.h"\
	".\mpilib.h"\
	".\random.h"\
	".\crypto.h"\
	".\fileio.h"\
	".\keymgmt.h"\
	".\rsagen.h"\
	".\mpiio.h"\
	".\language.h"\
	".\pgp.h"\
	".\md5.h"\
	".\charset.h"\
	".\keymaint.h"\
	".\idea.h"\
	".\exitpgp.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_KEYMG=\
	".\Aestuff.h"\
	".\MacPGP.h"\
	".\Macutil2.h"\
	".\Macutil3.h"\
	".\PGPDialogs.h"\
	".\password.h"\
	".\MyBufferedStdio.h"\
	".\ReplaceStdio.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\keymgmt.obj" : $(SOURCE) $(DEP_CPP_KEYMG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\keymgmt.obj" : $(SOURCE) $(DEP_CPP_KEYMG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\keymgmt.obj" : $(SOURCE) $(DEP_CPP_KEYMG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\keymgmt.obj" : $(SOURCE) $(DEP_CPP_KEYMG) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\system.c
DEP_CPP_SYSTE=\
	".\exitpgp.h"\
	".\system.h"\
	".\usuals.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\file.h"\
	".\pgp.h"\
	".\mpilib.h"\
	".\mpiio.h"\
	".\fileio.h"\
	".\charset.h"\
	{$(INCLUDE)}"\stdlib.h"\
	".\c370.h"\
	"S:\common\inc\vpragma.h"\
	".\more.h"\
	".\armor.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	
NODEP_CPP_SYSTE=\
	".\My_console.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\system.obj" : $(SOURCE) $(DEP_CPP_SYSTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\system.obj" : $(SOURCE) $(DEP_CPP_SYSTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\system.obj" : $(SOURCE) $(DEP_CPP_SYSTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\system.obj" : $(SOURCE) $(DEP_CPP_SYSTE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ztrees.c
DEP_CPP_ZTREE=\
	".\zip.h"\
	".\ztailor.h"\
	".\ziperr.h"\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\ztrees.obj" : $(SOURCE) $(DEP_CPP_ZTREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\ztrees.obj" : $(SOURCE) $(DEP_CPP_ZTREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\ztrees.obj" : $(SOURCE) $(DEP_CPP_ZTREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\ztrees.obj" : $(SOURCE) $(DEP_CPP_ZTREE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getopt.c
DEP_CPP_GETOP=\
	".\getopt.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\getopt.obj" : $(SOURCE) $(DEP_CPP_GETOP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\getopt.obj" : $(SOURCE) $(DEP_CPP_GETOP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\getopt.obj" : $(SOURCE) $(DEP_CPP_GETOP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\getopt.obj" : $(SOURCE) $(DEP_CPP_GETOP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zdeflate.c
DEP_CPP_ZDEFL=\
	".\zunzip.h"\
	".\zip.h"\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\system.h"\
	{$(INCLUDE)}"\sys\timeb.h"\
	{$(INCLUDE)}"\os2.h"\
	{$(INCLUDE)}"\file.h"\
	".\c370.h"\
	{$(INCLUDE)}"\sys\types.h"\
	"S:\common\inc\btosdef.h"\
	"S:\common\inc\btdos.h"\
	"S:\common\inc\BTSTRUCT.H"\
	"S:\common\inc\vpragma.h"\
	".\ztailor.h"\
	".\ziperr.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	
NODEP_CPP_ZDEFL=\
	".\Macutil3.h"\
	".\macstat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zdeflate.obj" : $(SOURCE) $(DEP_CPP_ZDEFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zdeflate.obj" : $(SOURCE) $(DEP_CPP_ZDEFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zdeflate.obj" : $(SOURCE) $(DEP_CPP_ZDEFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zdeflate.obj" : $(SOURCE) $(DEP_CPP_ZDEFL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\charset.c
DEP_CPP_CHARS=\
	".\usuals.h"\
	".\language.h"\
	".\charset.h"\
	".\system.h"\
	".\c370.h"\
	{$(INCLUDE)}"\sys\types.h"\
	{$(INCLUDE)}"\stdlib.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\charset.obj" : $(SOURCE) $(DEP_CPP_CHARS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\more.c
DEP_CPP_MORE_=\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\types.h"\
	".\system.h"\
	".\mpilib.h"\
	".\language.h"\
	".\fileio.h"\
	".\pgp.h"\
	".\more.h"\
	".\charset.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\armor.h"\
	
NODEP_CPP_MORE_=\
	".\Macutil3.h"\
	".\MacPGP.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\more.obj" : $(SOURCE) $(DEP_CPP_MORE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\more.obj" : $(SOURCE) $(DEP_CPP_MORE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\more.obj" : $(SOURCE) $(DEP_CPP_MORE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\more.obj" : $(SOURCE) $(DEP_CPP_MORE_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\language.c
DEP_CPP_LANGU=\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\fileio.h"\
	".\language.h"\
	".\pgp.h"\
	".\charset.h"\
	".\armor.h"\
	".\c370.h"\
	".\more.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\language.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zinflate.c
DEP_CPP_ZINFL=\
	".\zunzip.h"\
	".\exitpgp.h"\
	{$(INCLUDE)}"\stdlib.h"\
	".\usuals.h"\
	".\system.h"\
	{$(INCLUDE)}"\sys\timeb.h"\
	{$(INCLUDE)}"\os2.h"\
	{$(INCLUDE)}"\file.h"\
	".\c370.h"\
	{$(INCLUDE)}"\sys\types.h"\
	"S:\common\inc\btosdef.h"\
	"S:\common\inc\btdos.h"\
	"S:\common\inc\BTSTRUCT.H"\
	"S:\common\inc\vpragma.h"\
	
NODEP_CPP_ZINFL=\
	".\Macutil3.h"\
	".\macstat.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\zinflate.obj" : $(SOURCE) $(DEP_CPP_ZINFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\zinflate.obj" : $(SOURCE) $(DEP_CPP_ZINFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\zinflate.obj" : $(SOURCE) $(DEP_CPP_ZINFL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\zinflate.obj" : $(SOURCE) $(DEP_CPP_ZINFL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgp.c
DEP_CPP_PGP_C=\
	{$(INCLUDE)}"\stdlib.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	".\system.h"\
	".\mpilib.h"\
	".\random.h"\
	".\crypto.h"\
	".\fileio.h"\
	".\keymgmt.h"\
	".\language.h"\
	".\pgp.h"\
	".\exitpgp.h"\
	".\charset.h"\
	".\getopt.h"\
	".\config.h"\
	".\keymaint.h"\
	".\keyadd.h"\
	".\rsaglue.h"\
	".\noise.h"\
	{$(INCLUDE)}"\sys\types.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	".\idea.h"\
	".\more.h"\
	".\armor.h"\
	
NODEP_CPP_PGP_C=\
	".\Macutil.h"\
	".\Macutil2.h"\
	".\Macutil3.h"\
	".\Macutil4.h"\
	".\Macbinary.h"\
	".\Binhex.h"\
	".\MacPGP.h"\
	".\mystr.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\pgp.obj" : $(SOURCE) $(DEP_CPP_PGP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\pgp.obj" : $(SOURCE) $(DEP_CPP_PGP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\pgp.obj" : $(SOURCE) $(DEP_CPP_PGP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\pgp.obj" : $(SOURCE) $(DEP_CPP_PGP_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpw32asm.c
DEP_CPP_MPW32=\
	".\mpilib.h"\
	".\usuals.h"\
	".\platform.h"\
	".\mpw32asm.h"\
	".\c370.h"\
	

!IF  "$(CFG)" == "pgpwin32 - Win32 Portable Release"


"$(INTDIR)\mpw32asm.obj" : $(SOURCE) $(DEP_CPP_MPW32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Portable Debug"


"$(INTDIR)\mpw32asm.obj" : $(SOURCE) $(DEP_CPP_MPW32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Release"


"$(INTDIR)\mpw32asm.obj" : $(SOURCE) $(DEP_CPP_MPW32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pgpwin32 - Win32 Intel Debug"


"$(INTDIR)\mpw32asm.obj" : $(SOURCE) $(DEP_CPP_MPW32) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################

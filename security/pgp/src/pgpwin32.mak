# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Portable Debug
!MESSAGE No configuration specified.  Defaulting to Portable Debug.
!ENDIF 

!IF "$(CFG)" != "Portable Release" && "$(CFG)" != "Portable Debug" && "$(CFG)"\
 != "Intel Release" && "$(CFG)" != "Intel Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "pgpwin32.mak" CFG="Portable Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Portable Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Portable Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Intel Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Intel Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Portable Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Portable Release"

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

ALL : $(OUTDIR)/"pgp.exe" $(OUTDIR)/"pgpwin32.bsc"

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"pgpwin32.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"pgpwin32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/"zglobals.sbr" \
	$(INTDIR)/"random.sbr" \
	$(INTDIR)/"crypto.sbr" \
	$(INTDIR)/"idea.sbr" \
	$(INTDIR)/"zunzip.sbr" \
	$(INTDIR)/"randpool.sbr" \
	$(INTDIR)/"passwd.sbr" \
	$(INTDIR)/"zipup.sbr" \
	$(INTDIR)/"md5.sbr" \
	$(INTDIR)/"mpiio.sbr" \
	$(INTDIR)/"mpilib.sbr" \
	$(INTDIR)/"rsagen.sbr" \
	$(INTDIR)/"genprime.sbr" \
	$(INTDIR)/"zfile_io.sbr" \
	$(INTDIR)/"strtol.sbr" \
	$(INTDIR)/"zip.sbr" \
	$(INTDIR)/"keymaint.sbr" \
	$(INTDIR)/"config.sbr" \
	$(INTDIR)/"rsaglue1.sbr" \
	$(INTDIR)/"mdfile.sbr" \
	$(INTDIR)/"memmove.sbr" \
	$(INTDIR)/"zbits.sbr" \
	$(INTDIR)/"sleep.sbr" \
	$(INTDIR)/"keyadd.sbr" \
	$(INTDIR)/"armor.sbr" \
	$(INTDIR)/"fileio.sbr" \
	$(INTDIR)/"noise.sbr" \
	$(INTDIR)/"keymgmt.sbr" \
	$(INTDIR)/"system.sbr" \
	$(INTDIR)/"ztrees.sbr" \
	$(INTDIR)/"getopt.sbr" \
	$(INTDIR)/"zdeflate.sbr" \
	$(INTDIR)/"charset.sbr" \
	$(INTDIR)/"more.sbr" \
	$(INTDIR)/"language.sbr" \
	$(INTDIR)/"zinflate.sbr" \
	$(INTDIR)/"pgp.sbr" \
	$(INTDIR)/"mpw32asm.sbr"

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386 /OUT:"WinRel/pgp.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"pgpwin32.pdb" /MACHINE:I386 /OUT:"WinRel/pgp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/"zglobals.obj" \
	$(INTDIR)/"random.obj" \
	$(INTDIR)/"crypto.obj" \
	$(INTDIR)/"idea.obj" \
	$(INTDIR)/"zunzip.obj" \
	$(INTDIR)/"randpool.obj" \
	$(INTDIR)/"passwd.obj" \
	$(INTDIR)/"zipup.obj" \
	$(INTDIR)/"md5.obj" \
	$(INTDIR)/"mpiio.obj" \
	$(INTDIR)/"mpilib.obj" \
	$(INTDIR)/"rsagen.obj" \
	$(INTDIR)/"genprime.obj" \
	$(INTDIR)/"zfile_io.obj" \
	$(INTDIR)/"strtol.obj" \
	$(INTDIR)/"zip.obj" \
	$(INTDIR)/"keymaint.obj" \
	$(INTDIR)/"config.obj" \
	$(INTDIR)/"rsaglue1.obj" \
	$(INTDIR)/"mdfile.obj" \
	$(INTDIR)/"memmove.obj" \
	$(INTDIR)/"zbits.obj" \
	$(INTDIR)/"sleep.obj" \
	$(INTDIR)/"keyadd.obj" \
	$(INTDIR)/"armor.obj" \
	$(INTDIR)/"fileio.obj" \
	$(INTDIR)/"noise.obj" \
	$(INTDIR)/"keymgmt.obj" \
	$(INTDIR)/"system.obj" \
	$(INTDIR)/"ztrees.obj" \
	$(INTDIR)/"getopt.obj" \
	$(INTDIR)/"zdeflate.obj" \
	$(INTDIR)/"charset.obj" \
	$(INTDIR)/"more.obj" \
	$(INTDIR)/"language.obj" \
	$(INTDIR)/"zinflate.obj" \
	$(INTDIR)/"pgp.obj" \
	$(INTDIR)/"mpw32asm.obj"

$(OUTDIR)/"pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Portable Debug"

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

ALL : $(OUTDIR)/"pgp.exe" $(OUTDIR)/"pgpwin32.bsc"

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"pgpwin32.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"pgpwin32.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"pgpwin32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/"zglobals.sbr" \
	$(INTDIR)/"random.sbr" \
	$(INTDIR)/"crypto.sbr" \
	$(INTDIR)/"idea.sbr" \
	$(INTDIR)/"zunzip.sbr" \
	$(INTDIR)/"randpool.sbr" \
	$(INTDIR)/"passwd.sbr" \
	$(INTDIR)/"zipup.sbr" \
	$(INTDIR)/"md5.sbr" \
	$(INTDIR)/"mpiio.sbr" \
	$(INTDIR)/"mpilib.sbr" \
	$(INTDIR)/"rsagen.sbr" \
	$(INTDIR)/"genprime.sbr" \
	$(INTDIR)/"zfile_io.sbr" \
	$(INTDIR)/"strtol.sbr" \
	$(INTDIR)/"zip.sbr" \
	$(INTDIR)/"keymaint.sbr" \
	$(INTDIR)/"config.sbr" \
	$(INTDIR)/"rsaglue1.sbr" \
	$(INTDIR)/"mdfile.sbr" \
	$(INTDIR)/"memmove.sbr" \
	$(INTDIR)/"zbits.sbr" \
	$(INTDIR)/"sleep.sbr" \
	$(INTDIR)/"keyadd.sbr" \
	$(INTDIR)/"armor.sbr" \
	$(INTDIR)/"fileio.sbr" \
	$(INTDIR)/"noise.sbr" \
	$(INTDIR)/"keymgmt.sbr" \
	$(INTDIR)/"system.sbr" \
	$(INTDIR)/"ztrees.sbr" \
	$(INTDIR)/"getopt.sbr" \
	$(INTDIR)/"zdeflate.sbr" \
	$(INTDIR)/"charset.sbr" \
	$(INTDIR)/"more.sbr" \
	$(INTDIR)/"language.sbr" \
	$(INTDIR)/"zinflate.sbr" \
	$(INTDIR)/"pgp.sbr" \
	$(INTDIR)/"mpw32asm.sbr"

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386 /OUT:"WinDebug/pgp.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"pgpwin32.pdb" /DEBUG /MACHINE:I386 /OUT:"WinDebug/pgp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/"zglobals.obj" \
	$(INTDIR)/"random.obj" \
	$(INTDIR)/"crypto.obj" \
	$(INTDIR)/"idea.obj" \
	$(INTDIR)/"zunzip.obj" \
	$(INTDIR)/"randpool.obj" \
	$(INTDIR)/"passwd.obj" \
	$(INTDIR)/"zipup.obj" \
	$(INTDIR)/"md5.obj" \
	$(INTDIR)/"mpiio.obj" \
	$(INTDIR)/"mpilib.obj" \
	$(INTDIR)/"rsagen.obj" \
	$(INTDIR)/"genprime.obj" \
	$(INTDIR)/"zfile_io.obj" \
	$(INTDIR)/"strtol.obj" \
	$(INTDIR)/"zip.obj" \
	$(INTDIR)/"keymaint.obj" \
	$(INTDIR)/"config.obj" \
	$(INTDIR)/"rsaglue1.obj" \
	$(INTDIR)/"mdfile.obj" \
	$(INTDIR)/"memmove.obj" \
	$(INTDIR)/"zbits.obj" \
	$(INTDIR)/"sleep.obj" \
	$(INTDIR)/"keyadd.obj" \
	$(INTDIR)/"armor.obj" \
	$(INTDIR)/"fileio.obj" \
	$(INTDIR)/"noise.obj" \
	$(INTDIR)/"keymgmt.obj" \
	$(INTDIR)/"system.obj" \
	$(INTDIR)/"ztrees.obj" \
	$(INTDIR)/"getopt.obj" \
	$(INTDIR)/"zdeflate.obj" \
	$(INTDIR)/"charset.obj" \
	$(INTDIR)/"more.obj" \
	$(INTDIR)/"language.obj" \
	$(INTDIR)/"zinflate.obj" \
	$(INTDIR)/"pgp.obj" \
	$(INTDIR)/"mpw32asm.obj"

$(OUTDIR)/"pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Intel Release"

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

ALL : ".\Intel_Release\pgp.exe" $(OUTDIR)/"pgpwin32.bsc"

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "USE_WIN32_ASSEMBLER" /FR /Fp"Intel_Release/pgpwin32.pch" /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "USE_WIN32_ASSEMBLER" /FR$(INTDIR)/ /Fp"Intel_Release/pgpwin32.pch"\
 /Fo$(INTDIR)/ /c 
CPP_OBJS=.\Intel_Release/
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"pgpwin32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/"zglobals.sbr" \
	$(INTDIR)/"random.sbr" \
	$(INTDIR)/"crypto.sbr" \
	$(INTDIR)/"idea.sbr" \
	$(INTDIR)/"zunzip.sbr" \
	$(INTDIR)/"randpool.sbr" \
	$(INTDIR)/"passwd.sbr" \
	$(INTDIR)/"zipup.sbr" \
	$(INTDIR)/"md5.sbr" \
	$(INTDIR)/"mpiio.sbr" \
	$(INTDIR)/"mpilib.sbr" \
	$(INTDIR)/"rsagen.sbr" \
	$(INTDIR)/"genprime.sbr" \
	$(INTDIR)/"zfile_io.sbr" \
	$(INTDIR)/"strtol.sbr" \
	$(INTDIR)/"zip.sbr" \
	$(INTDIR)/"keymaint.sbr" \
	$(INTDIR)/"config.sbr" \
	$(INTDIR)/"rsaglue1.sbr" \
	$(INTDIR)/"mdfile.sbr" \
	$(INTDIR)/"memmove.sbr" \
	$(INTDIR)/"zbits.sbr" \
	$(INTDIR)/"sleep.sbr" \
	$(INTDIR)/"keyadd.sbr" \
	$(INTDIR)/"armor.sbr" \
	$(INTDIR)/"fileio.sbr" \
	$(INTDIR)/"noise.sbr" \
	$(INTDIR)/"keymgmt.sbr" \
	$(INTDIR)/"system.sbr" \
	$(INTDIR)/"ztrees.sbr" \
	$(INTDIR)/"getopt.sbr" \
	$(INTDIR)/"zdeflate.sbr" \
	$(INTDIR)/"charset.sbr" \
	$(INTDIR)/"more.sbr" \
	$(INTDIR)/"language.sbr" \
	$(INTDIR)/"zinflate.sbr" \
	$(INTDIR)/"pgp.sbr" \
	$(INTDIR)/"mpw32asm.sbr"

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386 /OUT:"WinRel/pgp.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /PDB:"Intel_Release/pgpwin32.pdb" /MACHINE:I386 /OUT:"Intel_Release\pgp.exe"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\
 /PDB:"Intel_Release/pgpwin32.pdb" /MACHINE:I386 /OUT:"Intel_Release\pgp.exe" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/"zglobals.obj" \
	$(INTDIR)/"random.obj" \
	$(INTDIR)/"crypto.obj" \
	$(INTDIR)/"idea.obj" \
	$(INTDIR)/"zunzip.obj" \
	$(INTDIR)/"randpool.obj" \
	$(INTDIR)/"passwd.obj" \
	$(INTDIR)/"zipup.obj" \
	$(INTDIR)/"md5.obj" \
	$(INTDIR)/"mpiio.obj" \
	$(INTDIR)/"mpilib.obj" \
	$(INTDIR)/"rsagen.obj" \
	$(INTDIR)/"genprime.obj" \
	$(INTDIR)/"zfile_io.obj" \
	$(INTDIR)/"strtol.obj" \
	$(INTDIR)/"zip.obj" \
	$(INTDIR)/"keymaint.obj" \
	$(INTDIR)/"config.obj" \
	$(INTDIR)/"rsaglue1.obj" \
	$(INTDIR)/"mdfile.obj" \
	$(INTDIR)/"memmove.obj" \
	$(INTDIR)/"zbits.obj" \
	$(INTDIR)/"sleep.obj" \
	$(INTDIR)/"keyadd.obj" \
	$(INTDIR)/"armor.obj" \
	$(INTDIR)/"fileio.obj" \
	$(INTDIR)/"noise.obj" \
	$(INTDIR)/"keymgmt.obj" \
	$(INTDIR)/"system.obj" \
	$(INTDIR)/"ztrees.obj" \
	$(INTDIR)/"getopt.obj" \
	$(INTDIR)/"zdeflate.obj" \
	$(INTDIR)/"charset.obj" \
	$(INTDIR)/"more.obj" \
	$(INTDIR)/"language.obj" \
	$(INTDIR)/"zinflate.obj" \
	$(INTDIR)/"pgp.obj" \
	$(INTDIR)/"mpw32asm.obj"

".\Intel_Release\pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Intel Debug"

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

ALL : ".\Intel_Debug\pgp.exe" $(OUTDIR)/"pgpwin32.bsc"

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

$(INTDIR) : 
    if not exist $(INTDIR)/nul mkdir $(INTDIR)

# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /c
# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "USE_WIN32_ASSEMBLER" /FR /Fp"Intel_Debug/pgpwin32.pch" /Fd"Intel_Debug/pgpwin32.pdb" /c
CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D\
 "USE_WIN32_ASSEMBLER" /FR$(INTDIR)/ /Fp"Intel_Debug/pgpwin32.pch" /Fo$(INTDIR)/\
 /Fd"Intel_Debug/pgpwin32.pdb" /c 
CPP_OBJS=.\Intel_Debug/
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"pgpwin32.bsc" 
BSC32_SBRS= \
	$(INTDIR)/"zglobals.sbr" \
	$(INTDIR)/"random.sbr" \
	$(INTDIR)/"crypto.sbr" \
	$(INTDIR)/"idea.sbr" \
	$(INTDIR)/"zunzip.sbr" \
	$(INTDIR)/"randpool.sbr" \
	$(INTDIR)/"passwd.sbr" \
	$(INTDIR)/"zipup.sbr" \
	$(INTDIR)/"md5.sbr" \
	$(INTDIR)/"mpiio.sbr" \
	$(INTDIR)/"mpilib.sbr" \
	$(INTDIR)/"rsagen.sbr" \
	$(INTDIR)/"genprime.sbr" \
	$(INTDIR)/"zfile_io.sbr" \
	$(INTDIR)/"strtol.sbr" \
	$(INTDIR)/"zip.sbr" \
	$(INTDIR)/"keymaint.sbr" \
	$(INTDIR)/"config.sbr" \
	$(INTDIR)/"rsaglue1.sbr" \
	$(INTDIR)/"mdfile.sbr" \
	$(INTDIR)/"memmove.sbr" \
	$(INTDIR)/"zbits.sbr" \
	$(INTDIR)/"sleep.sbr" \
	$(INTDIR)/"keyadd.sbr" \
	$(INTDIR)/"armor.sbr" \
	$(INTDIR)/"fileio.sbr" \
	$(INTDIR)/"noise.sbr" \
	$(INTDIR)/"keymgmt.sbr" \
	$(INTDIR)/"system.sbr" \
	$(INTDIR)/"ztrees.sbr" \
	$(INTDIR)/"getopt.sbr" \
	$(INTDIR)/"zdeflate.sbr" \
	$(INTDIR)/"charset.sbr" \
	$(INTDIR)/"more.sbr" \
	$(INTDIR)/"language.sbr" \
	$(INTDIR)/"zinflate.sbr" \
	$(INTDIR)/"pgp.sbr" \
	$(INTDIR)/"mpw32asm.sbr"

$(OUTDIR)/"pgpwin32.bsc" : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386 /OUT:"WinDebug/pgp.exe"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /PDB:"Intel_Debug/pgpwin32.pdb" /DEBUG /MACHINE:I386 /OUT:"Intel_Debug/pgp.exe"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\
 /PDB:"Intel_Debug/pgpwin32.pdb" /DEBUG /MACHINE:I386 /OUT:"Intel_Debug/pgp.exe"\
 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/"zglobals.obj" \
	$(INTDIR)/"random.obj" \
	$(INTDIR)/"crypto.obj" \
	$(INTDIR)/"idea.obj" \
	$(INTDIR)/"zunzip.obj" \
	$(INTDIR)/"randpool.obj" \
	$(INTDIR)/"passwd.obj" \
	$(INTDIR)/"zipup.obj" \
	$(INTDIR)/"md5.obj" \
	$(INTDIR)/"mpiio.obj" \
	$(INTDIR)/"mpilib.obj" \
	$(INTDIR)/"rsagen.obj" \
	$(INTDIR)/"genprime.obj" \
	$(INTDIR)/"zfile_io.obj" \
	$(INTDIR)/"strtol.obj" \
	$(INTDIR)/"zip.obj" \
	$(INTDIR)/"keymaint.obj" \
	$(INTDIR)/"config.obj" \
	$(INTDIR)/"rsaglue1.obj" \
	$(INTDIR)/"mdfile.obj" \
	$(INTDIR)/"memmove.obj" \
	$(INTDIR)/"zbits.obj" \
	$(INTDIR)/"sleep.obj" \
	$(INTDIR)/"keyadd.obj" \
	$(INTDIR)/"armor.obj" \
	$(INTDIR)/"fileio.obj" \
	$(INTDIR)/"noise.obj" \
	$(INTDIR)/"keymgmt.obj" \
	$(INTDIR)/"system.obj" \
	$(INTDIR)/"ztrees.obj" \
	$(INTDIR)/"getopt.obj" \
	$(INTDIR)/"zdeflate.obj" \
	$(INTDIR)/"charset.obj" \
	$(INTDIR)/"more.obj" \
	$(INTDIR)/"language.obj" \
	$(INTDIR)/"zinflate.obj" \
	$(INTDIR)/"pgp.obj" \
	$(INTDIR)/"mpw32asm.obj"

".\Intel_Debug\pgp.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\zglobals.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zglobals.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zglobals.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zglobals.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zglobals.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\random.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"random.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"random.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"random.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"random.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\crypto.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"crypto.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"crypto.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"crypto.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"crypto.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\idea.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"idea.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"idea.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"idea.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"idea.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zunzip.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zunzip.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zunzip.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zunzip.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zunzip.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\randpool.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"randpool.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"randpool.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"randpool.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"randpool.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\passwd.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"passwd.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"passwd.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"passwd.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"passwd.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zipup.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zipup.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zipup.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zipup.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zipup.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\md5.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"md5.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"md5.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"md5.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"md5.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpiio.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"mpiio.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"mpiio.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"mpiio.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"mpiio.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpilib.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"mpilib.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"mpilib.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"mpilib.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"mpilib.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rsagen.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"rsagen.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"rsagen.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"rsagen.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"rsagen.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\genprime.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"genprime.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"genprime.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"genprime.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"genprime.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zfile_io.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zfile_io.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zfile_io.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zfile_io.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zfile_io.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strtol.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"strtol.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"strtol.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"strtol.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"strtol.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zip.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zip.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zip.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zip.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zip.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keymaint.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"keymaint.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"keymaint.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"keymaint.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"keymaint.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\config.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"config.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"config.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"config.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"config.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rsaglue1.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"rsaglue1.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"rsaglue1.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"rsaglue1.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"rsaglue1.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mdfile.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"mdfile.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"mdfile.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"mdfile.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"mdfile.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memmove.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"memmove.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"memmove.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"memmove.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"memmove.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zbits.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zbits.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zbits.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zbits.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zbits.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\sleep.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"sleep.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"sleep.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"sleep.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"sleep.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keyadd.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"keyadd.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"keyadd.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"keyadd.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"keyadd.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\armor.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"armor.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"armor.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"armor.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"armor.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\fileio.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"fileio.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"fileio.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"fileio.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"fileio.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\noise.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"noise.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"noise.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"noise.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"noise.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\keymgmt.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"keymgmt.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"keymgmt.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"keymgmt.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"keymgmt.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\system.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"system.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"system.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"system.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"system.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ztrees.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"ztrees.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"ztrees.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"ztrees.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"ztrees.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getopt.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"getopt.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"getopt.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"getopt.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"getopt.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zdeflate.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zdeflate.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zdeflate.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zdeflate.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zdeflate.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\charset.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"charset.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"charset.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"charset.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"charset.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\more.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"more.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"more.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"more.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"more.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\language.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"language.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"language.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"language.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"language.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\zinflate.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"zinflate.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"zinflate.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"zinflate.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"zinflate.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\pgp.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"pgp.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"pgp.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"pgp.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"pgp.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mpw32asm.c

!IF  "$(CFG)" == "Portable Release"

$(INTDIR)/"mpw32asm.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Portable Debug"

$(INTDIR)/"mpw32asm.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Release"

$(INTDIR)/"mpw32asm.obj" :  $(SOURCE)  $(INTDIR)

!ELSEIF  "$(CFG)" == "Intel Debug"

$(INTDIR)/"mpw32asm.obj" :  $(SOURCE)  $(INTDIR)

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################

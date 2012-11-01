# Microsoft Developer Studio Project File - Name="MP3EncDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MP3EncDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MP3EncDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MP3EncDll.mak" CFG="MP3EncDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MP3EncDll - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MP3EncDll - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MP3EncDll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /Ox /Ot /Og /Oy /Ob2 /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_BLADEDLL"   /YX /FD -Qmem -Qip /c
# SUBTRACT CPP /Oa /Ow /Oi /Os
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"..\..\lame_enc.dll"

!ELSEIF  "$(CFG)" == "MP3EncDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MTd /W3 /Gm /GX /Zi /Od /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_BLADEDLL"   /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\lame_enc.dll" /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "MP3EncDll - Win32 Release"
# Name "MP3EncDll - Win32 Debug"
# Begin Source File

SOURCE=.\BladeMP3EncDLL.c
# End Source File
# Begin Source File

SOURCE=.\BladeMP3EncDLL.def
# End Source File
# Begin Source File

SOURCE=.\BladeMP3EncDLL.h
# End Source File
# Begin Source File

SOURCE=..\brhist.h
# End Source File
# Begin Source File

SOURCE=..\brhist.c
# End Source File
# Begin Source File

SOURCE=..\encoder.h
# End Source File
# Begin Source File

SOURCE=..\fft.c
# End Source File
# Begin Source File

SOURCE=..\fft.h
# End Source File
# Begin Source File

SOURCE=..\formatBitstream.c
# End Source File
# Begin Source File

SOURCE=..\formatBitstream.h
# End Source File
# Begin Source File

SOURCE=..\gpkplotting.c
# End Source File
# Begin Source File

SOURCE=..\gpkplotting.h
# End Source File
# Begin Source File

SOURCE=..\gtkanal.c
# End Source File
# Begin Source File

SOURCE=..\gtkanal.h
# End Source File
# Begin Source File

SOURCE=..\huffman.h
# End Source File
# Begin Source File

SOURCE=..\id3tag.c
# End Source File
# Begin Source File

SOURCE=..\id3tag.h
# End Source File
# Begin Source File

SOURCE=..\ieeefloat.c
# End Source File
# Begin Source File

SOURCE=..\ieeefloat.h
# End Source File
# Begin Source File

SOURCE="..\l3bitstream-pvt.h"
# End Source File
# Begin Source File

SOURCE=..\l3bitstream.c
# End Source File
# Begin Source File

SOURCE=..\l3bitstream.h
# End Source File
# Begin Source File

SOURCE=..\l3side.h
# End Source File
# Begin Source File

SOURCE=..\lame.c
# End Source File
# Begin Source File

SOURCE=..\lame.h
# End Source File
# Begin Source File

SOURCE=..\newmdct.c
# End Source File
# Begin Source File

SOURCE=..\newmdct.h
# End Source File
# Begin Source File

SOURCE=..\parse.c
# End Source File
# Begin Source File

SOURCE=..\portableio.c
# End Source File
# Begin Source File

SOURCE=..\portableio.h
# End Source File
# Begin Source File

SOURCE=..\psymodel.c
# End Source File
# Begin Source File

SOURCE=..\psymodel.h
# End Source File
# Begin Source File

SOURCE="..\quantize-pvt.c"
# End Source File
# Begin Source File

SOURCE="..\quantize-pvt.h"
# End Source File
# Begin Source File

SOURCE=..\quantize.c
# End Source File
# Begin Source File

SOURCE=..\quantize.h
# End Source File
# Begin Source File

SOURCE=..\reservoir.c
# End Source File
# Begin Source File

SOURCE=..\reservoir.h
# End Source File
# Begin Source File

SOURCE=..\sqrttab.h
# End Source File
# Begin Source File

SOURCE=..\tables.c
# End Source File
# Begin Source File

SOURCE=..\tables.h
# End Source File
# Begin Source File

SOURCE=..\timestatus.c
# End Source File
# Begin Source File

SOURCE=..\timestatus.h
# End Source File
# Begin Source File

SOURCE=..\takehiro.c
# End Source File
# Begin Source File

SOURCE=..\util.c
# End Source File
# Begin Source File

SOURCE=..\util.h
# End Source File
# Begin Source File

SOURCE=..\VbrTag.c
# End Source File
# Begin Source File

SOURCE=..\VbrTag.h
# End Source File
# Begin Source File

SOURCE=..\version.c
# End Source File
# Begin Source File

SOURCE=..\version.h
# End Source File
# Begin Source File

SOURCE=..\wavheader.h
# End Source File
# End Target
# End Project

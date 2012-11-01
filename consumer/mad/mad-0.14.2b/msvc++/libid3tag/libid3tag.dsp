# Microsoft Developer Studio Project File - Name="libid3tag" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libid3tag - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libid3tag.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libid3tag.mak" CFG="libid3tag - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libid3tag - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libid3tag - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libid3tag - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libid3tag - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Gm /GX /ZI /Od /I "." /D "_LIB" /D "HAVE_CONFIG_H" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "DEBUG" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libid3tag - Win32 Release"
# Name "libid3tag - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\libid3tag\compat.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\crc.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\debug.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\field.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\file.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\frame.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\frametype.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\genre.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\latin1.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\parse.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\render.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\tag.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\ucs4.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\utf16.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\utf8.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\util.c
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\libid3tag\compat.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\crc.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\field.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\file.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\frame.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\frametype.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\genre.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\global.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\id3tag.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\latin1.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\parse.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\render.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\tag.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\ucs4.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\utf16.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\utf8.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\util.h
# End Source File
# Begin Source File

SOURCE=..\..\libid3tag\version.h
# End Source File
# End Group
# Begin Group "Data Files"

# PROP Default_Filter "dat"
# Begin Source File

SOURCE=..\..\libid3tag\genre.dat
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="mpglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mpglib - Win32 Debug GTK
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mpglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpglib.mak" CFG="mpglib - Win32 Debug GTK"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mpglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mpglib - Win32 Debug GTK" (based on "Win32 (x86) Static Library")
!MESSAGE "mpglib - Win32 Release GTK" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl.exe

!IF  "$(CFG)" == "mpglib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"  /YX /FD /c
# ADD CPP /nologo /W3 /GX /Ox /Ot /Og /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "HAVEMPGLIB" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mpglib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mpglib - Win32 Debug GTK"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mpglib__"
# PROP BASE Intermediate_Dir "mpglib__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugGTK"
# PROP Intermediate_Dir "DebugGTK"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "../WinGTK/glib-1.2" /I "../WinGTK/gtk-plus" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "HAVEGTK" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mpglib - Win32 Release GTK"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mpglib_0"
# PROP BASE Intermediate_Dir "mpglib_0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseGTK"
# PROP Intermediate_Dir "ReleaseGTK"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../WinGTK/glib-1.2" /I "../WinGTK/gtk-plus" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "HAVEGTK" /D "HAVEMPGLIB" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mpglib - Win32 Release"
# Name "mpglib - Win32 Debug"
# Name "mpglib - Win32 Debug GTK"
# Name "mpglib - Win32 Release GTK"
# Begin Source File

SOURCE=.\common.c
# End Source File
# Begin Source File

SOURCE=.\dct64_i386.c
# End Source File
# Begin Source File

SOURCE=.\decode_i386.c
# End Source File
# Begin Source File

SOURCE=.\huffman.h
# End Source File
# Begin Source File

SOURCE=.\interface.c
# End Source File
# Begin Source File

SOURCE=.\layer3.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\mpg123.h
# End Source File
# Begin Source File

SOURCE=.\mpglib.h
# End Source File
# Begin Source File

SOURCE=.\tabinit.c
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="lame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=lame - Win32 Debug GTK
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lame.mak" CFG="lame - Win32 Debug GTK"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lame - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "lame - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "lame - Win32 Debug GTK" (based on "Win32 (x86) Console Application")
!MESSAGE "lame - Win32 Release GTK" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lame - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Ox /Ot /Og /I "./WinGTK/gtk-plus" /I "./WinGTK/glib-1.2" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "LAMEPARSE" /D "HAVEMPGLIB" /D "LAMESNDFILE"  /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /map /machine:I386

!ELSEIF  "$(CFG)" == "lame - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D BS_FORMAT=BINARY /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "lame - Win32 Debug GTK"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lame___W"
# PROP BASE Intermediate_Dir "lame___W"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugGTK"
# PROP Intermediate_Dir "DebugGTK"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /W3 /Gm /GX /Zi /Od /I "../WinGTK/gtk-plus" /I "../WinGTK/glib-1.2" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS"  /D "HAVEGTK" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "./WinGTK/gtk-plus" /I "./WinGTK/glib-1.2" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "LAMESNDFILE" /D "HAVEMPGLIB" /D "HAVEGTK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-1.2.lib gdk-1.3.lib gtk-1.3.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"./WinGTK/glib-1.2" /libpath:"./WinGTK/gtk-plus/gdk" /libpath:"./WinGTK/gtk-plus/gtk"

!ELSEIF  "$(CFG)" == "lame - Win32 Release GTK"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lame___0"
# PROP BASE Intermediate_Dir "lame___0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseGTK"
# PROP Intermediate_Dir "ReleaseGTK"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../WinGTK/gtk-plus" /I "../WinGTK/glib-1.2" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS"  /D "HAVEGTK" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./WinGTK/gtk-plus" /I "./WinGTK/glib-1.2" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "LAMEPARSE" /D "HAVEMPGLIB" /D "LAMESNDFILE" /D "HAVEGTK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /map /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-1.2.lib gdk-1.3.lib gtk-1.3.lib /nologo /subsystem:console /profile /map /machine:I386 /libpath:"./WinGTK/glib-1.2" /libpath:"./WinGTK/gtk-plus/gdk" /libpath:"./WinGTK/gtk-plus/gtk"

!ENDIF 

# Begin Target

# Name "lame - Win32 Release"
# Name "lame - Win32 Debug"
# Name "lame - Win32 Debug GTK"
# Name "lame - Win32 Release GTK"
# Begin Source File

SOURCE=.\encoder.h
# End Source File
# Begin Source File

SOURCE=.\fft.c
# End Source File
# Begin Source File

SOURCE=.\fft.h
# End Source File
# Begin Source File

SOURCE=.\formatBitstream.c
# End Source File
# Begin Source File

SOURCE=.\formatBitstream.h
# End Source File
# Begin Source File

SOURCE=.\get_audio.c
# End Source File
# Begin Source File

SOURCE=.\get_audio.h
# End Source File
# Begin Source File

SOURCE=.\gpkplotting.c
# End Source File
# Begin Source File

SOURCE=.\gpkplotting.h
# End Source File
# Begin Source File

SOURCE=.\gtkanal.c
# End Source File
# Begin Source File

SOURCE=.\gtkanal.h
# End Source File
# Begin Source File

SOURCE=.\id3tag.c
# End Source File
# Begin Source File

SOURCE=.\id3tag.h
# End Source File
# Begin Source File

SOURCE=.\ieeefloat.c
# End Source File
# Begin Source File

SOURCE=.\ieeefloat.h
# End Source File
# Begin Source File

SOURCE=".\l3bitstream-pvt.h"
# End Source File
# Begin Source File

SOURCE=.\l3bitstream.c
# End Source File
# Begin Source File

SOURCE=.\l3bitstream.h
# End Source File
# Begin Source File

SOURCE=.\l3side.h
# End Source File
# Begin Source File

SOURCE=.\lame.c
# End Source File
# Begin Source File

SOURCE=.\lame.h
# End Source File
# Begin Source File

SOURCE=.\machine.h
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\newmdct.c
# End Source File
# Begin Source File

SOURCE=.\newmdct.h
# End Source File
# Begin Source File

SOURCE=.\parse.c
# End Source File
# Begin Source File

SOURCE=.\portableio.c
# End Source File
# Begin Source File

SOURCE=.\portableio.h
# End Source File
# Begin Source File

SOURCE=.\psymodel.c
# End Source File
# Begin Source File

SOURCE=.\psymodel.h
# End Source File
# Begin Source File

SOURCE=".\quantize-pvt.c"
# End Source File
# Begin Source File

SOURCE=".\quantize-pvt.h"
# End Source File
# Begin Source File

SOURCE=.\quantize.c
# End Source File
# Begin Source File

SOURCE=.\quantize.h
# End Source File
# Begin Source File

SOURCE=.\reservoir.c
# End Source File
# Begin Source File

SOURCE=.\reservoir.h
# End Source File
# Begin Source File

SOURCE=.\tables.c
# End Source File
# Begin Source File

SOURCE=.\tables.h
# End Source File
# Begin Source File

SOURCE=.\takehiro.c
# End Source File
# Begin Source File

SOURCE=.\timestatus.c
# End Source File
# Begin Source File

SOURCE=.\timestatus.h
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\vbrquantize.c
# End Source File
# Begin Source File

SOURCE=.\VbrTag.c
# End Source File
# Begin Source File

SOURCE=.\VbrTag.h
# End Source File
# Begin Source File

SOURCE=.\version.c
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\WinGTK.txt
# End Source File
# End Target
# End Project

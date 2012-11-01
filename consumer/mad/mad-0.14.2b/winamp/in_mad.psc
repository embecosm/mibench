#
# mad - MPEG audio decoder
# Copyright (C) 2000-2001 Robert Leslie
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# $Id: in_mad.psc.in,v 1.3 2001/11/09 02:38:29 rob Exp $
#

# Pimpscript file (v1.3.0)

# Substitution variables:
#
#   $PROGRAMFILES  usually C:\Program Files
#   $WINDIR	   usually C:\Windows
#   $SYSDIR	   usually C:\Windows\System
#   $DESKTOP	   the desktop directory for the current user
#   $INSTDIR	   whatever the install directory ends up being
#   $VISDIR	   visualization plug-in directory (DO NOT USE in DefaultDir)
#   $DSPDIR	   dsp plug-in directory (DO NOT USE in DefaultDir)

Name MAD Plug-in for Winamp
Text This will install version 0.14.2b of the MAD input plug-in for Winamp on your computer.
OutFile MAD plug-in 0.14.2b.exe

# Files

SetOutPath $INSTDIR\Plugins

AddFile in_mad.dll
AddFile in_mad.txt

# Post-install

ExecFile $WINDIR\notepad.exe $INSTDIR\Plugins\in_mad.txt
#ExecFile $INSTDIR\winamp.exe

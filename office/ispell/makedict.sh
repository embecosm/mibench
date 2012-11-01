: Use /bin/sh
#
# $Id: makedict.sh,v 1.9 1994/01/25 07:11:53 geoff Exp $
#
# Copyright 1987, 1988, 1989, 1992, 1993, Geoff Kuenning, Granada Hills, CA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All modifications to the source code must be clearly marked as
#    such.  Binary redistributions based on modified source code
#    must be clearly marked as modified versions in the documentation
#    and/or other materials provided with the distribution.
# 4. All advertising materials mentioning features or use of this software
#    must display the following acknowledgment:
#      This product includes software developed by Geoff Kuenning and
#      other unpaid contributors.
# 5. The name of Geoff Kuenning may not be used to endorse or promote
#    products derived from this software without specific prior
#    written permission.
#
# THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
#	Make a beginning dictionary file for ispell, using an existing
#	speller.
#
#	Usage:
#
#	makedict file-list
#
#	The specified files are collected, split into words, and run through
#	the system speller (usually spell(1)).  Any words that the speller
#	accepts will be written to the standard output for use in making
#	an ispell dictionary.  Usually, you will want to run the output
#	of this script through "munchlist" to get a final dictionary.
#
# $Log: makedict.sh,v $
# Revision 1.9  1994/01/25  07:11:53  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#

# This program must produce a list of INCORRECTLY spelled words on standard
# output, given a list of words on standard input.  If you don't have a
# speller, but do have a lot of correctly-spelled files, try /bin/true.
#
SPELLPROG="${SPELLPROG-spell}"

TMP=${TMPDIR-/tmp}/mkdict$$

case "$#" in
    0)
	set X -
	shift
	;;
esac

trap "/bin/rm ${TMP}; exit 1" 1 2 15

cat "$@" | deroff | tr -cs "[A-Z][a-z]'" '[\012*]' | sort -uf -o ${TMP}
$SPELLPROG < ${TMP} | comm -13 - ${TMP}
/bin/rm ${TMP}

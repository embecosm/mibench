: Use /bin/sh
#
# $Id: tryaffix.X,v 1.6 1994/01/25 07:12:18 geoff Exp $
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
# Try out affixes to see if they produce valid roots
#
# Usage:
#
#	tryaffix [-p | -s] [-c] dict-file affix[+addition] ...
#
#	The -p and -s flags specify whether prefixes or suffixes
#	are being tried;  if neither is specified, suffixes are assumed.
#
#	If the -c flag is given, statistics on the various affixes are given:
#	a count of words it potentially applies to, and an estimate of the
#	number of dictionary bytes the flag would save.  The estimate will
#	be high if the flag generates words that are currently generated
#	by other flags.
#
#	The dictionary file, dict-file, must already be expanded and sorted,
#	and things will work best if uppercase has been folded to lower with
#	'tr'.
#
#	The "affixes" are things to be stripped from the dictionary
#	file to produce trial roots:  for English, "con" and "ing"
#	are examples.  The "additions" are letters that would have
#	been stripped off the root before adding the affix.  For
#	example, the affix "ing" strips "e" for words ending in "e"
#	(as in "like --> liking") so we might run:
#
#	    tryaffix ing ing+e
#
#	to cover both cases.
#
# $Log: tryaffix.X,v $
# Revision 1.6  1994/01/25  07:12:18  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#
SORTTMP="-T ${TMPDIR-/tmp}"		# !!SORTTMP!!
USAGE='tryaffix [-p | -s] [-c] dict-file affix[+addition] ...'
counts=no
pre=
suf='$'
while :
do
    case "$1" in
	-p)
	    pre='^'
	    suf=
	    ;;
	-s)
	    pre=
	    suf='$'
	    ;;
	-c)
	    counts=yes
	    ;;
	-*)
	    echo "$USAGE" 1>&2
	    exit 1
	    ;;
	*)
	    break
	    ;;
    esac
    shift
done
dict="$1"
shift
if [ ! -r "$dict" ]
then
    echo "Can't read $dict" 1>&2
    echo "$USAGE" 1>&2
    exit 1
elif [ $# -eq 0 ]
then
    echo "$USAGE" 1>&2
    exit 1
fi
while [ $# -ne 0 ]
do
    case "$1" in
	*+*)
	    affix=`expr "$1" : '\(.*\)+'`
	    addition=`expr "$1" : '.*+\(.*\)'`
	    sedscript="s/$pre$affix$suf/$addition/p"
	    ;;
	*)
	    sedscript="s/$pre$1$suf//p"
	    ;;
    esac
    if [ "$counts" = no ]
    then
	echo ===== "$1" =====
	sed -n "$sedscript" "$dict" | comm -12 - "$dict"
    else
	echo "$1" `sed -n "$sedscript" "$dict" | comm -12 - "$dict" | wc -lc`
    fi
    shift
done

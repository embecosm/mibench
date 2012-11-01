: Use /bin/sh
#
# $Id: zapdups.X,v 1.6 1995/01/08 23:23:58 geoff Exp $
#
# Copyright 1993, Geoff Kuenning, Granada Hills, CA
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
# Report or get rid of duplicates in various components of a dictionary.
#
# Usage:
#
#	zapdups [-d [-n]] [-l langfile] dict-0 dict-1 ...
#
# Dictionaries starting with dict-1 (not dict-0!) are examined,
# looking for words that appear in any earlier dictionary.  If any
# duplicates are found, they are reported to the standard output.
#
# If the -d switch is specified, duplicates are removed from later
# dictionaries.  The modification is done in-place.  This switch
# should normally be used after examining the output of an earlier
# run.  The -d switch takes a long time to run, because it uses
# munchlist to reduce the dictionary once duplicates are removed.
# The -n switch can be used to suppress the running of munchlist, to
# save time.
#
# If the -l switch is specified, the language tables are gotten from
# the specified file; otherwise they come from $LIBDIR/!!DEFLANG!!.
#
# $Log: zapdups.X,v $
# Revision 1.6  1995/01/08  23:23:58  geoff
# Support variable hashfile suffixes for DOS purposes.
#
# Revision 1.5  1994/01/25  07:12:24  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#
LIBDIR=!!LIBDIR!!
TDIR=${TMPDIR-/tmp}
TMP=${TDIR}/zd$$
SORTTMP="-T ${TDIR}"			# !!SORTTMP!!
USAGE="zapdups [-d [-n]] [-l langfile] dict-0 dict-1 ..."

delete=no
munchit=yes
langtabs=${LIBDIR}/!!DEFLANG!!
while :
do
    case "$1" in
	-d)
	    delete=yes
	    shift
	    ;;
	-l)
	    langtabs="$2"
	    shift; shift
	    ;;
	-n)
	    munchit=no
	    shift
	    ;;
	-*)
	    echo "$USAGE" 1>&2
	    exit 1
	    ;;
	*)
	    break
	    ;;
    esac
done

if [ $# -lt 2 ]
then
    echo "$USAGE" 1>&2
    exit 1
fi

FAKEHASH=$TMP.a!!HASHSUFFIX!!
FAKEDICT=$TMP.b
SEEN=$TMP.c
LATEST=$TMP.d
DUPS=$TMP.e

trap "rm -f $TMP.*; exit 1" 1 2 15
trap "rm -f $TMP.*; exit 0" 13

#
# Create a dummy dictionary to hold a compiled copy of the language
# tables.
#
echo 'QQQQQQQQ' > $FAKEDICT
buildhash -s $FAKEDICT $langtabs $FAKEHASH \
  ||  (echo "Couldn't create fake hash file" 1>&2; rm -f $TMP.*; exit 1) \
  ||  exit 1
rm -f ${FAKEDICT}*

nl='
'
#
# Expand dictionary 0 into a temp file
#
ispell -e -d $FAKEHASH < "$1" \
  | tr ' ' "$nl" \
  | sort $SORTTMP -u \
  | sed 's@$@	'"$1@" \
  > $SEEN
shift

#
# For each subsequent dictionary:
#
#	(1) Expand it into a temp file
#	(2) Use join to report the duplicates
#	(3) If we are editing, use comm to remove the duplicates
#	(4) Add the expanded dictionary (sans duplicates) to the list
#	    of words already seen.
#
for dict
do
    ispell -e -d $FAKEHASH < "$dict" \
      | tr ' ' "$nl" \
      | sort $SORTTMP -u \
      | sed 's@$@	'"$dict@" \
      > $LATEST
    join '-t	' $SEEN $LATEST > $DUPS
    if [ -s $DUPS ]
    then
	cat $DUPS
	if [ $delete = yes ]
	then
	    sed "s@	.*	$dict@	$dict@" $DUPS \
	      | comm -23 $LATEST - \
	      | sed "s@	$dict@@" \
	      | if [ $munchit = yes ]
		then
		    munchlist -l "$langtabs" > "$dict"
		else
		    sort $SORTTMP -u -o "$dict"
		fi
	fi
    fi
    # We must do a shift so that $# remains correct
    shift
    if [ $# -gt 0 ]
    then
	sort $SORTTMP -u -o $SEEN $LATEST $SEEN
    fi
done \
  | sort -u
rm -f $TMP.*

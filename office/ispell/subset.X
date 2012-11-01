: Use /bin/sh
#
# $Id: subset.X,v 1.15 1995/01/08 23:23:47 geoff Exp $
#
# Copyright 1992, 1993, Geoff Kuenning, Granada Hills, CA
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
#	Combine and resolve various dictionaries so they are proper
#	subsets of one another, and so that maximal use is made of
#	flags in the smaller ones.
#
#	Usage:
#
#	subset [-b base] [-l langfile] small-dict bigger-dict ... biggest-dict
#
#	The output is a an equal number of successively-larger
#	dictionaries.  The smallest is written to "dict.0".  Successive
#	files are named "dict.1", "dict.2", and so forth, and each contains
#	a list of words which should be added to the previous files to
#	generate a dictionary.  Words which are in smaller dictionaries are
#	effectively propagated to the larger ones, so that the smaller ones
#	are proper subsets of their siblings.  If dictionaries are
#	completely disjoint, this may result in an empty output dictionary.
#	Affix flags are propagated to the smallest dictionary containing
#	the root word;  this expands the effectiveness of small dictionaries
#	at no cost in hash table space.
#
#	The -b switch is used to specify a different base name for the
#	output files than "dict".  (In other words, "-b english" would
#	produce output in english.0, english.1, etc.).
#
#	If the -l switch is specified, the language tables are gotten
#	from the specified file;  otherwise they come from $LIBDIR/!!DEFLANG!!.
#
#	Input dictionaries should be "clean";  if non-word characters
#	appear in the dictionaries, the script may produce incorrect output.
#
# $Log: subset.X,v $
# Revision 1.15  1995/01/08  23:23:47  geoff
# Support variable hashfile suffixes for DOS purposes.
#
# Revision 1.14  1994/01/25  07:12:10  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#
LIBDIR=!!LIBDIR!!
TDIR=${TMPDIR-/tmp}
TMP=${TDIR}/sset$$.
SORTTMP="-T ${TDIR}"			# !!SORTTMP!!
USAGE="Usage:  subset [-b base] [-l langfile] dict-0 dict-1 ..."

langtabs=${LIBDIR}/!!DEFLANG!!
outbase=dict
while :
do
    case "$1" in
	-b)
	    outbase="$2"
	    shift; shift
	    ;;
	-l)
	    langtabs="$2"
	    shift; shift
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

# Temp files
MUNCHOUTPUT=${TMP}a
MISSINGWORDS=${TMP}b
TEMPDICT=${TMP}c
FAKEDICT=${TMP}d
FAKEHASH=${TMP}e!!HASHSUFFIX!!

trap "/bin/rm -f ${TMP}*; exit 1" 1 2 15
trap "/bin/rm -f ${TMP}*; exit 0" 13

#
# Create a dummy dictionary to hold a compiled copy of the language
# tables.
#
echo 'QQQQQQQQ' > $FAKEDICT
buildhash -s $FAKEDICT $langtabs $FAKEHASH \
  ||  (echo "Couldn't create fake hash file" 1>&2; /bin/rm -f ${TMP}*; exit 1) \
  ||  exit 1
/bin/rm -f ${FAKEDICT}*
#
# Figure out what the flag-marking character is.
#
flagmarker=`ispell -D -d $FAKEHASH \
  | sed -n '/^flagmarker/s/flagmarker //p'`
case "$flagmarker" in
    \\*)
	flagmarker=`expr "$flagmarker" : '.\(.\)'`
	;;
esac    
#
#	(1) Use munchlist to create a list of roots and maximal suffixes.
#
munchlist -l $langtabs "$@" | sort $SORTTMP > $MUNCHOUTPUT
#
#	(2) Use join to add the maximal suffixes to each dictionary's roots.
#	    Re-expand this, combine with the original, and save for later.
#
newline='
'
dictno=0
for dictfile
do
    ispell -e -d $FAKEHASH < $dictfile | tr ' ' "$newline" \
      | sort -u $SORTTMP | join "-t$flagmarker" -a1 - $MUNCHOUTPUT \
      | ispell -e -d $FAKEHASH | tr ' ' "$newline" \
      | sort -u $SORTTMP > ${TEMPDICT}.$dictno
    dictno=`expr $dictno + 1`
done
/bin/rm -f $MUNCHOUTPUT
#
#	(3) For each adjacent pair of dictionaries, use comm to find words
#	    in the smaller that are missing from the larger, and add them
#	    to the larger.
#
firstdict="$1"
shift
lastdict="${TEMPDICT}.0"
dictno=1
for dictfile
do
    comm -23 $lastdict ${TEMPDICT}.$dictno > $MISSINGWORDS.$dictno
    if [ -s $MISSINGWORDS.$dictno ]
    then
	sort $SORTTMP -o ${TEMPDICT}.$dictno \
	  ${TEMPDICT}.$dictno $MISSINGWORDS.$dictno
    fi
    lastdict="${TEMPDICT}.$dictno"
    dictno=`expr $dictno + 1`
done
/bin/rm -f $MISSINGWORDS.*
#
#	(4) For each pair of dictionaries, use comm to eliminate words in
#	    the smaller from the larger, and shrink the result with munchlist.
#	    From this point out, we ignore interrupts.
#
munchlist ${TEMPDICT}.0 > $outbase.0
lastdict="${TEMPDICT}.0"
dictno=1
trap "" 1 2 13 15
for dictfile
do
    comm -13 $lastdict ${TEMPDICT}.$dictno \
      | munchlist -l $langtabs > $outbase.$dictno
    /bin/rm -f $lastdict
    lastdict="${TEMPDICT}.$dictno"
    dictno=`expr $dictno + 1`
done
/bin/rm -f ${TMP}*

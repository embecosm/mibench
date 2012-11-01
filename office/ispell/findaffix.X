: Use /bin/sh
#
# $Id: findaffix.X,v 1.15 1994/01/25 07:11:29 geoff Exp $
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
#	Find possible affixes for use with ispell
#
#	Usage:
#
#	findaffix [-p | -s] [-f] [-c] [-m min] [-M max] [-e elim] [-l low] \
#	  [-t tabchar] [files]
#
#	Each common prefix (-p) or suffix (-s, default) is presented, along
#	with statistics to indicate how useful such an affix might be in
#	reducing the size of the input file.  Only those affixes which
#	produce a legal root (one found in the original input) are reported.
#
#	If the "-c" option is not given, the output lines are in the
#	following format:
#
#		strip/add/count/bytes
#
#	where "strip" is the string that should be stripped from a root
#	word before adding the affix, "add" is the affix to be added, "count"
#	is a count of the number of times that this "strip/add" combination
#	appears, and "bytes" is an estimate of the number of bytes that
#	will be saved in the raw dictionary file if this combination is
#	added to the affix file.  The field separator in the output will
#	normally be the tab character specified by the "-t" switch;  the
#	default is a slash ("/").
#
#	If the "-c" ("clean output") option is given, the appearance of
#	the output is made cleaner by changing it to:
#
#		-strip+add<tab>count<tab>bytes
#
#	where "strip," "add," "count," and "bytes" are as before, and "<tab>"
#	represents the ASCII tab character.
#
#	The method used to generate possible affixes will also generate
#	longer affixes which have common headers or trailers.  For example,
#	the two words "moth" and "mother" will generate not only the obvious
#	substition "+er" but also "-h+her" and "-th+ther" (and possibly
#	even longer ones, depending on the value of "min").  To prevent
#	cluttering the output with such affixes, any affix pair that shares
#	a common header (or, for prefixes, trailer) string longer than
#	"elim" characters (default 1) will be suppressed.  You may want to
#	set "elim" to a value greater than 1 if your language has string
#	characters;  usually the need for this parameter will become obvious
#	when you examine the output of your findaffix run.
#
#	Normally, the output is sorted on the "bytes" field.  If the "-f"
#	flag is given, the output is sorted according to the "count" field.
#
#	No affix longer than "max" characters (default 8) will be reported.
#	Smaller values of "max" will make the script run faster.
#
#	Affixes which appear fewer than "low" times (default 10) are
#	suppressed.  This significantly reduces the size of the output file.
#
#	Affixes which generate stems shorter than "min" characters (default 3)
#	are suppressed.  (A stem is the word after the "strip" string has
#	been removed, and before the "add" string has been added.)  This
#	reduces both the running time and the size of the output file.  "Min"
#	should only be set to 1 if you have a *lot* of free time and disk
#	space.
#
#	The script requires a non-blank field-separator character for internal
#	use.  Normally, this character is a slash ("/"), but if the slash
#	appears as a character in the input word list, a different character
#	can be specified with the "-t" switch.
#
#	If the input files are ispell dictionaries, they should be expanded
#	before being fed to this script.
#
#	If the input files contains characters other than [A-Za-z], they
#	should be translated to lowercase before being fed to this script.
#
# $Log: findaffix.X,v $
# Revision 1.15  1994/01/25  07:11:29  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#
TDIR=${TMPDIR-/tmp}
TMP=${TDIR}/faff$$
SORTTMP="-T ${TDIR}"			# !!SORTTMP!!
USAGE='Usage:  findaffix [-p | -s] [-f] [-c] [-e elim] [-m min] [-M max] [-l low] [-t tabch] [files]'
LOOP='
    i = len - maxlim + 1
    if (i < minstem + 1)
	i = minstem + 1
    for (  ;  i <= len;  i++)
	print substr ($0, 1, i - 1) tabch substr ($0, i) tabch len
    print $0 tabch tabch len'
ELIM='$1!=$2 \
    {
    if (substr ($1, 1, elimlen) != substr ($2, 1, elimlen))
	print
    }'
maxlim=8
minstem=3
elimlen=1
lowcount=10
cleanout=no
finalsortopts='+3rn -4 +2rn -3 +1 -2 +0 -1'
tabch=/
while :
do
    case "$1" in
	-p)
	    LOOP='
		lim = len - minstem
		if (lim > maxlim)
		    lim = maxlim
		for (i = 1;  i <= lim;  i++)
		    print substr ($0, i + 1) tabch substr ($0, 1, i) tabch len
		print $0 tabch tabch len'
	    ELIM='$1!=$2 \
		{
		if (substr ($1, length ($1), elimlen) \
		  != substr ($2, length ($2), elimlen))
		    print
		}'
	    shift
	    ;;
	-s)
	    shift
	    ;;
	-f)
	    finalsortopts='+2rn -3 +3rn -4 +1 -2 +0 -1'
	    shift
	    ;;
	-c)
	    cleanout=yes
	    shift
	    ;;
	-e)
	    elimlen=$2
	    shift; shift
	    ;;
	-m)
	    minstem=$2
	    shift; shift
	    ;;
	-M)
	    maxlim=$2
	    shift; shift
	    ;;
	-l)
	    lowcount=$2
	    shift; shift
	    ;;
	-t)
	    tabch="$2"
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
trap "/bin/rm -f ${TMP}*; exit 1" 1 2 15
trap "/bin/rm -f ${TMP}*; exit 0" 13
#
# We are ready to do the work.  First, we collect all input, translate it
# to lowercase, sort it (dropping duplications), and save it for later.
#
if [ $# -ne 0 ]
then
    cat "$@" | tr '[A-Z]' '[a-z]'
else
    tr '[A-Z]' '[a-z]'
fi \
  | sort -u $SORTTMP > ${TMP}a
#
# Now the monstrous pipeline.  The awk command produces several lines for
# each input word.  Each line contains a possible stem (first field),
# a possible affix, and the length of the original word.  The loop which
# does this was placed into the LOOP variable by the code above (q.v.).
#
# The first sort puts this output into an order appropriate for feeding
# to 'join'.  The join command then combines stems and affixes, and for
# each puts out an affix to strip, an affix to add, and the length of
# the word before and after modification.
#
# From here on out the job is relatively easy.  The second 'awk' gets rid
# of lines that have the same strip and add affixes, and also eliminates
# lines where the strip and add affix have a common leading (for suffixes)
# or trailing (for prefixes) substring, or where the strip affix is longer
# than the add affix (this is all done by the $ELIM variable, which is also
# set up by the code above.  The second sort collects identical affixes;
# the third 'awk' functions like 'uniq -c', replacing duplicate affixes
# with a count and summing the estimate of bytes saved.  It also eliminates
# any affixes which appear less frequently than the minimum ("lowcount").
# Finally, the third sort ($finalsortopts) rearranges the list in the chosen
# sort order.
#
awk "BEGIN{minstem=$minstem; maxlim=$maxlim; tabch="'"'"$tabch"'"}
    {
    len = length ($0)
    if (len < 2)
	next
    '"$LOOP"'
    }' < ${TMP}a \
  | sort "-t$tabch" +0 -1 +1 $SORTTMP -o ${TMP}a
join "-t$tabch" -o 1.2 2.2 2.3 ${TMP}a ${TMP}a \
  | awk "-F$tabch" "BEGIN{elimlen=$elimlen}$ELIM" \
  | sort "-t$tabch" +1 -2 +0 -1 $SORTTMP \
  | awk "-F$tabch" 'BEGIN{tabch="'"$tabch"'"; lowcount='"$lowcount"'}
	{
	if ($1 == last1  &&  $2 == last2)
	    {
	    count++
	    totchars += $3
	    }
	else
	    {
	    if ((last1 != ""  ||  last2 != "")  &&  count >= lowcount)
		print last1 tabch last2 tabch count tabch totchars
	    count = 1
	    last1 = $1
	    last2 = $2
	    totchars = $3
	    }
	}
    END {
	if ((last1 != ""  ||  last2 != "")  &&  count >= lowcount)
	    print last1 tabch last2 tabch count tabch totchars
	}' \
  | sort "-t$tabch" $finalsortopts $SORTTMP \
  | if [ "$cleanout" = "yes" ]
    then
	case "$tabch" in
	    /)
		sedsub=/
		sedsep=';'
		;;
	    .|\*|\[|\^|\$|\\)
		sedsub="\\$tabch"
		sedsep=/
		;;
	    *)
		sedsub="$tabch"
		sedsep=/
		;;
	esac
	exec sed -e "s$sedsep$sedsub$sedsep	${sedsep}g" \
	  -e 's/	/+/' -e 's/^/-/' \
	  -e 's/^-+/+/' -e 's/+	/	/'
    else
	exec cat
    fi
/bin/rm -f ${TMP}?

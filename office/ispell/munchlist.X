: Use /bin/sh
#
# $Id: munchlist.X,v 1.53 1995/01/08 23:23:36 geoff Exp $
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
#	Given a list of words for ispell, generate a reduced list
#	in which all possible affixes have been collapsed.  The reduced
#	list will match the same list as the original.
#
#	Usage:
#
#	munchlist [-l lang] [-c lang] [-s hashfile] [-D] [-w chars] [-v] \
#	  [file] ...
#
#	Options:
#
#	-l lang	Specifies the language table to be used.  The default
#		is "$LIBDIR/!!DEFLANG!!".
#	-c lang	Specifies "conversion" language table.  If this option is
#		given, the input file(s) will be assumed to be described by
#		this table, rather than the table given in the -l option.
#		This may be used to convert between incompatible language
#		tables.  (When in doubt, use this option -- it doesn't
#		hurt, and it may save you from creating a dictionary that has
#		illegal words in it).  The default is no conversion.
#	-T suff Specifies that the source word lists are in the format
#		of a "suff"-suffixed file, rather than in the
#		canonical form.  For example, "-T tex" specifies that
#		string characters in the word lists are in TeX format.
#		The string character conversions are taken from the language
#		table specified by the "-l" switch.
#	-s	Remove any words that are already covered by the
#		dictionary in 'hashfile'.  The words will be removed
#		only if all affixes are covered.  This option should not be
#		specified when the main dictionary is being munched.
#		'Hashfile' must have been created with the language
#		table given in the -l option, but this is not checked.
#	-D	Leave temporary files for debugging purposes
#	-w	Passed on to ispell (specify chars that are part of a word)
#		Unfortunately, special characters must be quoted twice
#		rather than once when invoking this script.  Also, since
#		buildhash doesn't accept this option, the final ispell -l
#		step ignores it, making it somewhat less than useful.
#	-v	Report progress to stderr.
#
#	The given input files are merged, then processed by 'ispell -c'
#	to generate possible affix lists;  these are then combined
#	and reduced.  The final result is written to standard output.
#
#	For portability to older systems, I have avoided getopt.
#
#		Geoff Kuenning
#		2/28/87
#
# $Log: munchlist.X,v $
# Revision 1.53  1995/01/08  23:23:36  geoff
# Support variable hashfile suffixes for DOS purposes.
#
# Revision 1.52  1994/12/27  23:08:46  geoff
# Dynamically determine how to pass backslashes to 'tr' so that it'll
# work on any machine.  Define LC_CTYPE to work around yet more
# internationalized sort programs.  Work around a bug in GNU uniq that
# uses the wrong separator between counts and duplicated lines.
#
# Revision 1.51  1994/11/21  07:02:54  geoff
# Correctly quote the arguments to 'tr' when detecting systems with
# unsigned sorts.  Be sure to provide a zero exit status on all systems,
# even if MUNCHDEBUG is not set.
#
# Revision 1.50  1994/10/25  05:46:05  geoff
# Export values for LANG and LOCALE in an attempt to override some
# stupidly-internationalized sort programs.
#
# Revision 1.49  1994/10/04  03:51:30  geoff
# Add the MUNCHMAIL feature.  If the MUNCHMAIL environment variable is
# set to an email address, debugging information about the munchlist run
# will automatically be collected and mailed to that address.
#
# Revision 1.48  1994/05/17  06:32:06  geoff
# Don't look for affix tables in LIBDIR if the name contains a slash
#
# Revision 1.47  1994/04/27  02:50:48  geoff
# Fix some cosmetic flaws in the verbose-mode messages.
#
# Revision 1.46  1994/01/25  07:11:59  geoff
# Get rid of all old RCS log lines in preparation for the 3.1 release.
#
#
if [ "X$MUNCHMAIL" != X ]
then
    exec 2> /tmp/munchlist.mail
    echo "munchlist $*" 1>&2
    set -vx
fi
LIBDIR=!!LIBDIR!!
TDIR=${TMPDIR-/tmp}
TMP=${TDIR}/munch$$
SORTTMP="-T ${TDIR}"			# !!SORTTMP!!
if [ -r ./icombine ]
then
    COMBINE=./icombine
else
    COMBINE=icombine
fi
if [ -r ./ijoin ]
then
    JOIN=./ijoin
else
    JOIN=ijoin
fi

#
# The following is necessary so that some internationalized versions of
# sort(1) don't confuse things by sorting into a nonstandard order.
#
LANG=C
LOCALE=C
LC_CTYPE=C
export LANG LOCALE LC_CTYPE

debug=no
dictopt=
langtabs=${LIBDIR}/!!DEFLANG!!
convtabs=
strip=no
icflags=
verbose=false
# The following value of "wchars" is necessary to prevent ispell from
# receiving a null argument if -w is not specified.  As long as "A" is
# a member of the existing character set, ispell will ignore the argument.
wchars=-wA
while [ $# != 0 ]
do
    case "$1" in
	-l)
	    case "$2" in
		*/*)
		    langtabs=$2
		    ;;
		*)
		    if [ -r "$2" ]
		    then
			langtabs="$2"
		    else
			langtabs="${LIBDIR}/$2"
		    fi
		    ;;
	    esac
	    if [ ! -r "$langtabs" ]
	    then
		echo "Can't open language table '$2'" 1>&2
		exit 1
	    fi
	    shift
	    ;;
	-c)
	    if [ -r "$2" ]
	    then
		convtabs="$2"
	    elif [ -r "${LIBDIR}/$2" ]
	    then
		convtabs="${LIBDIR}/$2"
	    else
		echo "Can't open conversion language table '$2'" 1>&2
		exit 1
	    fi
	    shift
	    ;;
	-s)
	    dictopt="-d $2"
	    strip=yes
	    shift
	    ;;
	-D)
	    debug=yes
	    ;;
	-T)
	    icflags="-T $2"
	    shift
	    ;;
	-v)
	    verbose=true
	    ;;
	-w)
	    wchars="-w$2"
	    shift
	    ;;
	--)
	    shift
	    break
	    ;;
	-)
	    break
	    ;;
	-*)
	    echo 'Usage: munchlist [-l lang] [-c lang] [-T suff] [-s hashfile] [-D] [-w chars] [-v] [file] ...' \
	      1>&2
	    exit 2
	    ;;
	*)
	    break
	    ;;
    esac
    shift
done
if [ "X$MUNCHMAIL" != X ]
then
    verbose=true
    debug=yes
fi
trap "/bin/rm -f ${TMP}*; exit 1" 1 2 13 15
#
# Names of temporary files.  This is just to make the code a little easier
# to read.
#
EXPANDEDINPUT=${TMP}a
STRIPPEDINPUT=${TMP}b
CRUNCHEDINPUT=${TMP}c
PRODUCTLIST=${TMP}d
EXPANDEDPAIRS=${TMP}e
LEGALFLAGLIST=${TMP}f
JOINEDPAIRS=${TMP}g
MINIMALAFFIXES=${TMP}h
CROSSROOTS=${TMP}i
CROSSEXPANDED=${TMP}j
CROSSPAIRS=${TMP}k
CROSSILLEGAL=${TMP}l
ILLEGALCOMBOS=${TMP}m
FAKEDICT=${TMP}n
# Ispell insists that hash files have a "!!HASHSUFFIX!!" suffix
FAKEHASH=${TMP}o!!HASHSUFFIX!!
AWKSCRIPT=${TMP}p
if [ "$debug" = yes ]
then
    touch $EXPANDEDINPUT $STRIPPEDINPUT $CRUNCHEDINPUT $PRODUCTLIST \
      $EXPANDEDPAIRS $LEGALFLAGLIST $JOINEDPAIRS $MINIMALAFFIXES \
      $CROSSROOTS $CROSSEXPANDED $CROSSPAIRS $CROSSILLEGAL $ILLEGALCOMBOS \
      $FAKEDICT $FAKEHASH $AWKSCRIPT
    rm -f ${TDIR}/EXPANDEDINPUT ${TDIR}/STRIPPEDINPUT ${TDIR}/CRUNCHEDINPUT \
      ${TDIR}/PRODUCTLIST ${TDIR}/EXPANDEDPAIRS ${TDIR}/LEGALFLAGLIST \
      ${TDIR}/JOINEDPAIRS ${TDIR}/MINIMALAFFIXES ${TDIR}/CROSSROOTS \
      ${TDIR}/CROSSEXPANDED ${TDIR}/CROSSPAIRS ${TDIR}/CROSSILLEGAL \
      ${TDIR}/ILLEGALCOMBOS ${TDIR}/FAKEDICT ${TDIR}/FAKEHASH!!HASHSUFFIX!! \
      ${TDIR}/AWKSCRIPT ${TDIR}/CROSSROOTS.[0-9]* ${TDIR}/CROSSEXP.[0-9]* \
      ${TDIR}/CROSSPAIRS.[0-9]* ${TDIR}/CROSSILLEGAL.[0-9]*
    ln $EXPANDEDINPUT ${TDIR}/EXPANDEDINPUT
    ln $STRIPPEDINPUT ${TDIR}/STRIPPEDINPUT
    ln $CRUNCHEDINPUT ${TDIR}/CRUNCHEDINPUT
    ln $PRODUCTLIST ${TDIR}/PRODUCTLIST
    ln $EXPANDEDPAIRS ${TDIR}/EXPANDEDPAIRS
    ln $LEGALFLAGLIST ${TDIR}/LEGALFLAGLIST
    ln $JOINEDPAIRS ${TDIR}/JOINEDPAIRS
    ln $MINIMALAFFIXES ${TDIR}/MINIMALAFFIXES
    ln $CROSSROOTS ${TDIR}/CROSSROOTS
    ln $CROSSEXPANDED ${TDIR}/CROSSEXPANDED
    ln $CROSSPAIRS ${TDIR}/CROSSPAIRS
    ln $CROSSILLEGAL ${TDIR}/CROSSILLEGAL
    ln $ILLEGALCOMBOS ${TDIR}/ILLEGALCOMBOS
    ln $FAKEDICT ${TDIR}/FAKEDICT
    ln $FAKEHASH ${TDIR}/FAKEHASH!!HASHSUFFIX!!
    ln $AWKSCRIPT ${TDIR}/AWKSCRIPT
fi
#
# Create a dummy dictionary to hold a compiled copy of the language
# table.  Initially, it holds the conversion table, if it exists.
#
case "X$convtabs" in
    X)
	convtabs="$langtabs"
	;;
esac
echo 'QQQQQQQQ' > $FAKEDICT
buildhash -s $FAKEDICT $convtabs $FAKEHASH \
  ||  (echo "Couldn't create fake hash file" 1>&2; /bin/rm -f ${TMP}*; exit 1) \
  ||  exit 1
#
# Figure out how 'sort' sorts signed fields, for arguments to ijoin.
# This is a little bit of a tricky pipe, but the result is that SIGNED
# is set to "-s" if characters with the top bit set sort before those
# without, and "-u" if the reverse is true.  How does it work?  The
# first "tr" step generates two lines, one containing "-u", the other
# with the same but with the high-order bit set.  The second "tr"
# changesthe high-bit "-u" back to "-s".  If the high-bit "-u" was
# sorted first, the sed step will select "-s" for SIGNED; otherwise
# it'll pick "-u".  We have to be careful about backslash quoting
# conventions, because some systems differ.
#
backslash=\\
for i in 0 1 2 3
do
    if [ `echo a | tr "${backslash}141" b` = b ]
    then
	break
    fi
    backslash="$backslash$backslash"
done
SIGNED=`echo '-s
-u' | tr s "${backslash}365" | sort | tr "${backslash}365" s | sed 1q`
#
# Collect all the input and expand all the affix options (ispell -e),
# and preserve (sorted) for later joining in EXPANDEDINPUT.  The icombine
# step is to make sure that unneeded capitalizations (e.g., Farmer and farmer)
# are weeded out.  The first sort must be folded for icombine;  the second
# must be unfolded for join.
#
$verbose  &&  echo "Collecting input." 1>&2
if [ $# -eq 0 ]
then
    ispell "$wchars" -e1 -d $FAKEHASH -p /dev/null | tr " " '
'
else
    cat "$@" | ispell "$wchars" -e1 -d $FAKEHASH -p /dev/null | tr " " '
'
fi \
  | sort $SORTTMP -u +0f -1 +0 \
  | $COMBINE $icflags $langtabs \
  | sort $SORTTMP -u > $EXPANDEDINPUT
#
# If a conversion table existed, recreate the fake hash file with the
# "real" language table.
#
case "$convtabs" in
    $langtabs)
	;;
    *)
	buildhash -s $FAKEDICT $langtabs $FAKEHASH \
	  ||  (echo "Couldn't create fake hash file" 1>&2; \
		/bin/rm -f ${TMP}*; exit 1) \
	  ||  exit 1
	;;
esac
/bin/rm -f ${FAKEDICT}*
#
# If the -s (strip) option was specified, remove all
# expanded words that are covered by the dictionary.  This produces
# the final list of expanded words that this dictionary must cover.
# Leave the list in STRIPPEDINPUT.
#
if [ "X$strip" = "Xno" ]
then
    rm -f $STRIPPEDINPUT
    ln $EXPANDEDINPUT $STRIPPEDINPUT
    if [ "$debug" = yes ]
    then
	rm -f ${TDIR}/STRIPPEDINPUT
	ln $STRIPPEDINPUT ${TDIR}/STRIPPEDINPUT
    fi
else
    $verbose  &&  echo "Stripping words already in the dictionary." 1>&2
    ispell "$wchars" -l $dictopt -p /dev/null < $EXPANDEDINPUT \
      > $STRIPPEDINPUT
fi
#
# Figure out what the flag-marking character is.
#
$verbose  &&  echo "Finding flag marker." 1>&2
flagmarker=`ispell -D -d $FAKEHASH \
  | sed -n '/^flagmarker/s/flagmarker //p'`
case "$flagmarker" in
    \\*)
	flagmarker=`expr "$flagmarker" : '.\(.\)'`
	;;
esac    
#
# Munch the input to generate roots and affixes (ispell -c).  We are
# only interested in words that have at least one affix (egrep $flagmarker);
# the next step will pick up the rest.  Some of the roots are illegal.  We
# use join to restrict the output to those root words that are found
# in the original dictionary.
#
$verbose  &&  echo "Generating roots and affixes." 1>&2
ispell "$wchars" -c -W0 -d $FAKEHASH -p /dev/null < $STRIPPEDINPUT \
  | tr " " '
' \
  | egrep "$flagmarker" | sort $SORTTMP -u "-t$flagmarker" +0 -1 +1 \
  | $JOIN $SIGNED "-t$flagmarker" - $EXPANDEDINPUT > $CRUNCHEDINPUT
#
# We now have a list of legal roots, and of affixes that apply to the
# root words.  However, it is possible for some affix flags to generate more
# than one output word.  For example, with the flag table entry
#
#	flag R:	. > ER
#		. > ERS
#
# the input "BOTHER" will generate an entry "BOTH/R" in CRUNCHEDINPUT.  But
# this will accept "BOTHER" and "BOTHERS" in the dictionary, which is
# wrong (in this case, though it's good English).
#
# To cure this problem, we first have to know which flags generate which
# expansions.  We use ispell -e3 to expand the flags (the second e causes
# the root and flag to be included in the output), and get pairs
# suitable for joining.  In the example above, we would get
#
#	BOTH/R BOTHER
#	BOTH/R BOTHERS
#
# We save this in EXPANDEDPAIRS for the next step.
#
$verbose  &&  echo 'Expanding dictionary into EXPANDEDPAIRS.' 1>&2
ispell "$wchars" -e3 -d $FAKEHASH -p /dev/null < $CRUNCHEDINPUT \
  | sort $SORTTMP +1 > $EXPANDEDPAIRS
#
# Now we want to extract the lines in EXPANDEDPAIRS in which the second field
# is *not* listed in the original dictionary EXPANDEDINPUT;  these illegal
# lines contain the flags we cannot include without accepting illegal words.
# It is somewhat easier to extract those which actually are listed (with
# join), and then use comm to strip these from EXPANDEDPAIRS to get the
# illegal expansions, together with the flags that generate them (we must
# re-sort EXPANDEDPAIRS before running comm).  Sed
# gets rid of the expansion and uniq gets rid of duplicates.  Comm then
# selects the remainder of the list from CRUNCHEDINPUT and puts it in
# LEGALFLAGLIST.  The final step is to use a sort and icombine to put
# the list into a one-entry-per-root format.
#
# BTW, I thought of using cut for the sed step (on systems that have it),
# but it turns out that sed is faster!
#
$JOIN -j1 2 -o 1.1 1.2 $SIGNED $EXPANDEDPAIRS $EXPANDEDINPUT \
  | sort $SORTTMP -u > $JOINEDPAIRS

sort $SORTTMP -o $EXPANDEDPAIRS $EXPANDEDPAIRS
sort $SORTTMP -o $CRUNCHEDINPUT $CRUNCHEDINPUT

$verbose  &&  echo 'Creating list of legal roots/flags.' 1>&2
comm -13 $JOINEDPAIRS $EXPANDEDPAIRS \
  | (sed -e 's; .*$;;' ; /bin/rm -f $JOINEDPAIRS $EXPANDEDPAIRS) \
  | uniq \
  | (comm -13 - $CRUNCHEDINPUT ; /bin/rm -f $CRUNCHEDINPUT) \
  | sort $SORTTMP -u "-t$flagmarker" +0f -1 +0 \
  | $COMBINE $langtabs > $LEGALFLAGLIST

#
# LEGALFLAGLIST now contains root/flag combinations that, when expanded,
# produce only words from EXPANDEDPAIRS.  However, there is still a
# problem if the language tables have any cross-product flags.  A legal
# root may appear in LEGALFLAGLIST with two flags that participate
# in cross-products.  When such a dictionary entry is expanded,
# the cross-products will generate some extra words that may not
# be in EXPANDEDPAIRS.  We need to remove these from LEGALFLAGLIST.
#
# The first step is to collect the names of the flags that participate
# in cross-products.  Ispell will dump the language tables for us, and
# sed is a pretty handy way to strip out extra information.  We use
# uniq -c and a numerical sort to put the flags in approximate order of how
# "productive" they are (in terms of how likely they are to generate a lot
# of output words).  The least-productive flags are given last and will
# be removed first.
#
$verbose \
  &&  echo 'Creating list of flags that participate in cross-products.' 1>&2
ispell -D -d $FAKEHASH \
  | sed -n '1,$s/:.*$//
    /^flagmarker/d
    /^prefixes/,/^suffixes/s/^  flag \*/p /p
    /^suffixes/,$s/^  flag \*/s /p' \
  | sort $SORTTMP \
  | uniq -c \
  | tr '	' ' ' \
  | sort $SORTTMP +0rn -1 +2 > $PRODUCTLIST

if [ `egrep ' p ' $PRODUCTLIST | wc -l` -gt 0 \
  -a `egrep ' s ' $PRODUCTLIST | wc -l` -gt 0 ]
then
    #
    # The language tables allow cross products.  See if LEGALFLAGLIST has
    # any roots with multiple cross-product flags.  Put them in CROSSROOTS.
    #
    $verbose  &&  echo 'Finding prefix and suffix flags.' 1>&2
    preflags=`sed -n 's/^[ 0-9]*p //p' $PRODUCTLIST | tr -d '
'`
    sufflags=`sed -n 's/^[ 0-9]*s //p' $PRODUCTLIST | tr -d '
'`
    egrep "$flagmarker.*[$preflags].*[$sufflags]|$flagmarker.*[$sufflags].*[$preflags]" \
      $LEGALFLAGLIST \
      > $CROSSROOTS

    #
    # We will need an awk script;  it's so big that it core-dumps my shell
    # under certain conditions.  The rationale behind the script is commented
    # where the script is used.  Note that you may want to change this
    # script for languages other than English.
    #
    case "$flagmarker" in
	/)
	    sedchar=:
	    ;;
	*)
	    sedchar=/
	    ;;
    esac
    $verbose  &&  echo 'Creating awk script.' 1>&2
    sed -e "s/PREFLAGS/$preflags/" -e "s/SUFFLAGS/$sufflags/" \
      -e "s;ILLEGALCOMBOS;$ILLEGALCOMBOS;" \
      -e "s${sedchar}FLAGMARKER${sedchar}$flagmarker${sedchar}" \
      > $AWKSCRIPT << 'ENDOFAWKSCRIPT'
	BEGIN \
	    {
	    preflags = "PREFLAGS"
	    sufflags = "SUFFLAGS"
	    illegalcombos = "ILLEGALCOMBOS"
	    flagmarker = "FLAGMARKER"
	    pflaglen = length (preflags)
	    for (i = 1;  i <= pflaglen;  i++)
		pflags[i] = substr (preflags, i, 1);
	    sflaglen = length (sufflags)
	    for (i = 1;  i <= sflaglen;  i++)
		sflags[i] = substr (sufflags, i, 1);
	    }
	    {
	    len = length ($2)
	    pnew2 = ""
	    snew2 = ""
	    pbad = ""
	    sbad = ""
	    sufs = 0
	    pres = 0
	    for (i = 1;  i <= len;  i++)
		{
		curflag = substr ($2, i, 1)
		for (j = 1;  j <= pflaglen;  j++)
		    {
		    if (pflags[j] == curflag)
			{
			pres++
			pnew2 = substr ($2, 1, i - 1) substr ($2, i + 1)
			pbad = curflag
			}
		    }
		for (j = 1;  j <= sflaglen;  j++)
		    {
		    if (sflags[j] == curflag)
			{
			sufs++
			snew2 = substr ($2, 1, i - 1) substr ($2, i + 1)
			sbad = curflag
			}
		    }
		}
	    if (pres == 1)
		{
		print $1 flagmarker pnew2
		print $1 flagmarker pbad >> illegalcombos
		}
	    else if (sufs == 1)
		{
		print $1 flagmarker snew2
		print $1 flagmarker sbad >> illegalcombos
		}
	    else if (pres > 0)
		{
		print $1 flagmarker pnew2
		print $1 flagmarker pbad >> illegalcombos
		}
	    else
		{
		print $1 flagmarker snew2
		print $1 flagmarker sbad >> illegalcombos
		}
	    }
ENDOFAWKSCRIPT
    : > $ILLEGALCOMBOS
    dbnum=0
    while [ -s $CROSSROOTS ]
    do
	#
	# CROSSROOTS contains the roots whose cross-product expansions
	# might be illegal.  We now need to locate the actual illegal ones.
	# We do this in much the same way we created LEGALFLAGLIST from
	# CRUNCHEDINPUT.  First we make CROSSEXPANDED, which is analogous
	# to EXPANDEDPAIRS.
	#
	$verbose  &&  echo "Creating cross expansions (pass $dbnum)." 1>&2
	ispell "$wchars" -e3 -d $FAKEHASH -p /dev/null < $CROSSROOTS \
	  | sort $SORTTMP +1 > $CROSSEXPANDED
	#
	# Now we join CROSSEXPANDED against EXPANDEDINPUT to produce
	# CROSSPAIRS, and then comm that against CROSSEXPANDED to
	# get CROSSILLEGAL, the list of illegal cross-product flag
	# combinations.
	#
	$JOIN -j1 2 -o 1.1 1.2 $SIGNED $CROSSEXPANDED $EXPANDEDINPUT \
	  | sort $SORTTMP -u > $CROSSPAIRS

	sort $SORTTMP -u -o $CROSSEXPANDED $CROSSEXPANDED

	$verbose \
	  &&  echo "Finding illegal cross expansions (pass $dbnum)." 1>&2
	comm -13 $CROSSPAIRS $CROSSEXPANDED \
	  | sed -e 's; .*$;;' \
	  | uniq > $CROSSILLEGAL

	if [ "$debug" = yes ]
	then
	    mv $CROSSROOTS $TDIR/CROSSROOTS.$dbnum
	    ln $CROSSEXPANDED $TDIR/CROSSEXP.$dbnum
	    ln $CROSSPAIRS $TDIR/CROSSPAIRS.$dbnum
	    ln $CROSSILLEGAL $TDIR/CROSSILLEGAL.$dbnum
	fi
	#
	# Now it is time to try to clear up the illegalities.  For 
	# each word in the illegal list, remove one of the cross-product
	# flags.  The flag chosen is selected in an attempt to cure the
	# problem quickly, as follows:  (1) if there is only one suffix
	# flag or only one prefix flag, we remove that.  (2) If there is
	# a prefix flag, we remove the "least desirable" (according to
	# the order of preflags). (This may be pro-English prejudice,
	# and you might want to change this if your language is prefix-heavy).
	# (3) Otherwise we remove the least-desirable suffix flag
	#
	# The output of the awk script becomes the new CROSSROOTS.  In
	# addition, we add the rejected flags to ILLEGALCOMBOS (this is done
	# inside the awk script) so they can be removed from LEGALFLAGLIST
	# later.
	#
	awk "-F$flagmarker" -f $AWKSCRIPT $CROSSILLEGAL > $CROSSROOTS
	if [ "$debug" = yes ]
	then
	    /bin/rm -f $CROSSEXPANDED $CROSSPAIRS $CROSSILLEGAL
	fi
	dbnum=`expr $dbnum + 1`
    done
    /bin/rm -f $CROSSEXPANDED $CROSSPAIRS $CROSSILLEGAL $AWKSCRIPT
    #
    # Now we have, in ILLEGALCOMBOS, a list of root/flag combinations
    # that must be removed from LEGALFLAGLIST to get the final list
    # of truly legal flags.  ILLEGALCOMBOS has one flag per line, so
    # by turning LEGALFLAGLIST into this form (sed), it's an
    # easy task for comm.  We have to recombine flags again after the
    # extraction, to get all flags for a given root on the same line so that
    # cross-products will come out right.
    #
    if [ -s $ILLEGALCOMBOS ]
    then
	sort $SORTTMP -u -o $ILLEGALCOMBOS $ILLEGALCOMBOS
	$verbose  &&  echo 'Finding roots of cross expansions.' 1>&2
	sort $SORTTMP $LEGALFLAGLIST \
	  | sed '/\/../{
	      s;^\(.*\)/\(.\)\(.*\);\1/\2\
\1/\3;
	      P
	      D
	      }' \
	  | comm -23 - $ILLEGALCOMBOS \
	  | sort $SORTTMP -u "-t$flagmarker" +0f -1 +0 \
	  | $COMBINE $langtabs > $CROSSROOTS
	mv $CROSSROOTS $LEGALFLAGLIST
	if [ "$debug" = yes ]
	then
	    rm -f ${TDIR}/LEGALFLAGLIST1
	    ln $LEGALFLAGLIST ${TDIR}/LEGALFLAGLIST1
	fi
    fi
fi
/bin/rm -f $PRODUCTLIST $CROSSROOTS $ILLEGALCOMBOS $EXPANDEDINPUT
#

# We now have (in LEGALFLAGLIST) a list of roots and flags which will
# accept words taken from EXPANDEDINPUT and no others (though some of
# EXPANDEDINPUT is not covered by this list).  However, many of the
# expanded words can be generated in more than one way.  For example,
# "bather" can be generated from "bath/R" and "bathe/R".  This wastes
# unnecessary space in the raw dictionary and, in some cases, in the
# hash file as well.  The solution is to list the various ways of
# getting a given word and choose exactly one.  All other things being
# equal, we want to choose the one with the highest expansion length
# to root length ratio.  The ispell -e4 option takes care of this by
# providing us with a field to sort on.
#
# The ispell/awk combination is similar to the ispell/sed pipe used to
# generate EXPANDEDPAIRS, except that ispell adds an extra field
# giving the sort order.  The first sort gets things in order so the
# first root listed is the one we want, and the second sort (-um) then
# selects that first root.  Sed strips the expansion from the root,
# and a final sort -u generates MINIMALAFFIXES, the final list of
# affixes that (more or less) minimally covers what it can from
# EXPANDEDINPUT.
#
$verbose  &&  echo 'Eliminating non-optimal affixes.' 1>&2
ispell "$wchars" -e4 -d $FAKEHASH -p /dev/null < $LEGALFLAGLIST \
  | sort $SORTTMP +1 -2 +2rn -3 +0 -1 \
  | sort $SORTTMP -um +1 -2 \
  | sed -e 's; .*$;;' \
  | sort $SORTTMP -u "-t$flagmarker" +0f -1 +0 > $MINIMALAFFIXES
/bin/rm -f $LEGALFLAGLIST
#
# Now we're almost done.  MINIMALAFFIXES covers some (with luck, most)
# of the words in STRIPPEDINPUT.  Now we must create a list of the remaining
# words (those omitted by MINIMALAFFIXES) and add it to MINIMALAFFIXES.
# The best way to do this is to actually build a partial dictionary from
# MINIMALAFFIXES in FAKEHASH, and then use ispell -l to list the words that
# are not covered by this dictionary.  This must then be combined with the
# reduced version of MINIMALAFFIXES and sorted to produce the final result.
#
$verbose  &&  echo "Generating output word list." 1>&2
if [ -s $MINIMALAFFIXES ]
then
    buildhash -s $MINIMALAFFIXES $langtabs $FAKEHASH > /dev/null \
      ||  (echo "Couldn't create intermediate hash file" 1>&2;
	/bin/rm -f ${TMP}*;
	exit 1) \
      ||  exit 1
    if [ "$debug" = yes ]
    then
	rm -f ${TDIR}/MINAFFIXES.!!COUNTSUFFIX!! \
	  ${TDIR}/MINAFFIXES!!STATSUFFIX!!
	ln $MINIMALAFFIXES.!!COUNTSUFFIX!! ${TDIR}/MINAFFIXES.!!COUNTSUFFIX!!
	ln $MINIMALAFFIXES!!STATSUFFIX!! ${TDIR}/MINAFFIXES!!STATSUFFIX!!
    fi
    (ispell "$wchars" -l -d $FAKEHASH -p /dev/null < $STRIPPEDINPUT; \
	$COMBINE $langtabs < $MINIMALAFFIXES) \
      | sort $SORTTMP "-t$flagmarker" -u +0f -1 +0
else
    # MINIMALAFFIXES is empty;  just produce a sorted version of STRIPPEDINPUT
    sort $SORTTMP "-t$flagmarker" -u +0f -1 +0 $STRIPPEDINPUT
fi
/bin/rm -f ${TMP}*
if [ "X$MUNCHMAIL" != X ]
then
    (
    ls -ld ${TDIR}/[A-Z]*
    cat /tmp/munchlist.mail
    ) | mail "$MUNCHMAIL"
    /bin/rm -f /tmp/munchlist.mail
fi
exit 0

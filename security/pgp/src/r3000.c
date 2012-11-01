/*	r3000.c - MIPS R3000 adaptation of selected routines from mpilib.c.

	C source code for multiprecision arithmetic library routines.
	R3000 optimizations by Castor Fu, in Sep 1992.
	See the comments in the file mpilib.c for details on the purpose of
	these functions.

	Original version of mpilib.c implemented in 1986 by 
	Philip R. Zimmermann, updated by PRZ 1990-1992.

	Boulder Software Engineering
	3021 Eleventh Street
	Boulder, CO 80304
	(303) 541-0140

	(c) Copyright 1990-1996 by Philip Zimmermann.  All rights reserved.
	The author assumes no liability for damages resulting from the use
	of this software, even if the damage results from defects in this
	software.  No warranty is expressed or implied.

	Note that while most PGP source modules bear Philip Zimmermann's
	copyright notice, many of them have been revised or entirely written
	by contributors who frequently failed to put their names in their
	code.  Code that has been incorporated into PGP from other authors
	was either originally published in the public domain or is used with
	permission from the various authors.

	PGP is available for free to the public under certain restrictions.
	See the PGP User's Guide (included in the release package) for
	important information about licensing, patent restrictions on
	certain algorithms, trademarks, copyrights, and export controls.

	-- Adapt so that the unit size can be a full int size. This
	   was inspired by the lack of a carry bit on MIPSco processors.
	   On such machines, the advantage of assembly language coding
	   is less clear.  (Except for the multiply. . . )

	   One reason for creating an R3000 module of C routines is that
	   we have one routine (P_ROTL) which is compiled particularly
	   poorly, and chose to explicitly unroll the loop.

*/


#include "mpilib.h"

#define word_v(r,n)	(r)[tohigher(n)]

extern short global_precision; /* units of precision for all routines */
/*	global_precision is the unit precision last set by set_precision.
	Initially, set_precision() should be called to define global_precision
	before using any of these other multiprecision library routines.
	i.e.:	set_precision(MAX_UNIT_PRECISION);
*/

/*************** multiprecision library primitives ****************/
/*	The following portable C primitives should be recoded in assembly.
	The equivalent assembly primitives are externally defined under
	different names, unless PORTABLE is defined.  See the header file
	"mpilib.h" for further details.
*/

typedef unsigned long int ulint;
/* ...assumes sizeof(unit) <= sizeof(unsigned long) */

boolean mp_addc
	(register unitptr r1,register unitptr r2,register boolean carry)
	/* multiprecision add with carry r2 to r1, result in r1 */
	/* carry is incoming carry flag-- value should be 0 or 1 */
{
	register unit x,x1;
	int i;
	short precision;	/* number of units to add */
	unsigned int mcarry = carry;
	precision = global_precision;
	make_lsbptr(r1,precision);
	make_lsbptr(r2,precision);

	i = precision&3;
	while (i--) {	
		  if (mcarry) {
		     x = *r1 + *r2 + 1;
		     x1 = ~ *r1;
		     mcarry = 1 ^ (*r2  < x1);
		  } else {
		     x = *r1 + *r2;
		     mcarry = (x < *r1) ;
		  }
		post_higherunit(r2);
		*post_higherunit(r1) = x;
	}

	i = precision>>2;
	while (i--) {	
#define ADDC(n) \
		  if (mcarry) { \
		     x = word_v(r1,n) + word_v(r2,n) + 1; \
		     x1 = ~word_v(r1,n); \
		     mcarry = 1 ^ (word_v(r2,n)  < x1); \
		  } else { \
		     x = word_v(r1,n) + word_v(r2,n); \
		     mcarry = (x < word_v(r1,n)) ; \
		  } \
		  word_v(r1,n) = x;

		  ADDC(0);
		  ADDC(1);
		  ADDC(2);
		  ADDC(3);
#undef ADDC
		r1 += tohigher(4);
		r2 += tohigher(4);
	}

	return(mcarry);		/* return the final carry flag bit */
}	/* mp_addc */


boolean mp_subb
	(register unitptr r1,register unitptr r2,register boolean borrow)
	/* multiprecision subtract with borrow, r2 from r1, result in r1 */
	/* borrow is incoming borrow flag-- value should be 0 or 1 */
{
	register unit x;	
	unsigned int mborrow = borrow;
	int i; 
	short precision;	/* number of units to subtract */
	precision = global_precision;
	make_lsbptr(r1,precision);
	make_lsbptr(r2,precision);

	i = precision&3;
	while (i--) {
		if (mborrow) {
		  x = *r1 - *r2 - mborrow;
		  mborrow = 1 ^ (*r2 < *r1);
	        } else {
		  x = *r1 - *r2;
		  mborrow = (*r1 < *r2);
		}
		post_higherunit(r2);
		*post_higherunit(r1) = x;
	}

	i = precision>>2;
	while (i--) {	

#define	SUBB(n)	\
		if (mborrow) { \
		  x = word_v(r1,n) - word_v(r2,n) - mborrow; \
		  mborrow = 1 ^ (word_v(r2,n) < word_v(r1,n)); \
	        } else { \
		  x = word_v(r1,n) - word_v(r2,n); \
		  mborrow = (word_v(r1,n) < word_v(r2,n)); \
		} \
		word_v(r1,n) = x;

		SUBB(0);
		SUBB(1);
		SUBB(2);
		SUBB(3);
#undef	SUBB


		r1 += tohigher(4);
		r2 += tohigher(4);
	}

	return(mborrow);	/* return the final carry/borrow flag bit */
}	/* mp_subb */


/* We've unrolled the loop here because the MIPS/DEC C compiler is too
 * stupid to do so.  Presumably on register poor machines this is not
 * a clearly good idea
 */

boolean mp_rotate_left(register unitptr r1,register boolean carry)
	/* multiprecision rotate left 1 bit with carry, result in r1. */
	/* carry is incoming carry flag-- value should be 0 or 1 */
{
	register int precision;	/* number of units to rotate */
	unsigned int mcarry = carry, carry2,carry3,carry4, nextcarry; 

	int i;
	precision = global_precision;
	make_lsbptr(r1,precision);
	i = precision &3;
	while (i--) {
		nextcarry = (((signedunit) *r1) < 0);
		*r1 = (*r1 << 1) | mcarry;
		mcarry = nextcarry;
		pre_higherunit(r1);
	}
	i = precision>>2;
	while (i--) {
		carry2 = (((signedunit) *r1) < 0);
		*r1 = (*r1 << 1) | mcarry;

		carry3 = (((signedunit) word_v(r1,1)) < 0);
		word_v(r1,1) = (word_v(r1,1) << 1) | carry2;

		carry4 = (((signedunit) word_v(r1,2)) < 0);
		word_v(r1,2) = (word_v(r1,2) << 1) | carry3;

		mcarry = (((signedunit) word_v(r1,3)) < 0);
		word_v(r1,3) = (word_v(r1,3) << 1) | carry4;

		r1 += tohigher(4);
	}
	return(mcarry);	/* return the final carry flag bit */
}	/* mp_rotate_left */

void p_setp(short nbits){} 

/************** end of primitives that should be in assembly *************/

#include "lmul.h"	/* used only by R3000.c */

#ifdef MUNIT16
typedef unsigned short MULTUNIT;
#endif

#ifdef MUNIT32
typedef unsigned int MULTUNIT;
#endif
#define	MULTUNITSIZE	(sizeof(MULTUNIT)*8)
#define MULTUNIT_hmask	((1UL << (MULTUNITSIZE/2))-1)
#define MULTUNIT_mask   ((MULTUNIT_hmask<<(MULTUNITSIZE/2)) | MULTUNIT_hmask)

void p_smula (
MULTUNIT *prod,
MULTUNIT *multiplicand,
MULTUNIT multiplier)
{
	short i=global_precision;
	int count=i,j;
	MULTUNIT *pp=prod, *mp=multiplicand;  
	MULTUNIT pl, ph, npl, nph, cl, ch;
	MULTUNIT al, ah;

	cl = 0;
	ch = 0;
	
	if (i <= 0 ) return;
	lmul(multiplier, *multiplicand, pl, ph);
	post_higherunit(multiplicand);
	al = 0;
	ah = 0;
	ch = 0;
	while(--i) {
		lmul(multiplier, *multiplicand, npl, nph);
		post_higherunit(multiplicand);
		al += *prod;
		cl = (al < *prod);
		al += pl;
		cl += (al < pl);
		ah += ph;
		ch = (ah < ph);
		ah += cl;
		ch += (ah < cl);
		*prod = al;
		post_higherunit(prod);
		al = ah;
		ah = ch;
		pl = npl;
		ph = nph;
	}
	al += *prod;
	cl = (al < *prod);
	al += pl;
	cl += (al < pl);
	ah += ph;
	ch = (ah < ph);
	ah += cl;
	ch += (ah < cl);

	*prod = al;
	post_higherunit(prod);

	*prod += ah;
	ch += (*prod < ah);
	post_higherunit(prod);

	/*
	*prod += ch ;
	post_higherunit(prod);
	*/


}	/* mp_smul */



static  int  mshift;			/* number of bits for
					** recip scaling	  */
static  MULTUNIT reciph; 		/* MSunit of scaled recip */
static  MULTUNIT recipl; 		/* LSunit of scaled recip */

void p_setrecip(MULTUNIT rh, MULTUNIT rl, int m)
{
	reciph = rh;
	recipl = rl;
	mshift = m;
}


MULTUNIT p_quo_digit (MULTUNIT *dividend) 
{
	MULTUNIT ql, qh, q0l, q0h, q1l, q1h, q2l, q2h;
	MULTUNIT q3h, q3l, q4h, q4l;
	MULTUNIT mutemp;


/*	Compute the least significant product group.
	The last terms of q1 and q2 perform upward rounding, which is
	needed to guarantee that the result not be too small.
*/
	lmul(dividend[tohigher(-2)] ^ MULTUNIT_mask, reciph, q1l, q1h);
	q1l += reciph;
	q1h += (q1l <  reciph);

	lmul(dividend[tohigher(-1)]^ MULTUNIT_mask, recipl, q2l, q2h);
	q2h += 1;

	q1l = (q1l >> 1) + (q1h << (MULTUNITSIZE-1));
	q1h >>= 1;
	q2l =  (q2l >> 1) + (q2h << (MULTUNITSIZE-1));
	q2h >>= 1;

	q0l = q1l + q2l;
	q0h = q1h + q2h + (q0l < q1l);

	q0l++;
	q0h+= (q0l < 1);

/*	Compute the middle significant product group.	*/

	lmul(dividend[tohigher(-1)]^MULTUNIT_mask, reciph, q3l, q3h);
	lmul(dividend[0] ^ MULTUNIT_mask, recipl,  q4l, q4h);

	q3l = (q3l >> 1) + (q3h << (MULTUNITSIZE-1));
	q3h >>= 1;
	q4l =  (q4l >> 1) + (q4h << (MULTUNITSIZE-1));
	q4h >>= 1;

	ql = q0h + 1;
	qh = (ql < 1);

	ql += q3l;
	qh += q3h + (ql < q3l);
	ql += q4l;
	qh += q4h + (ql < q4l);

/*	Compute the most significant term and add in the others */

	lmul((dividend[0] ^ MULTUNIT_mask), reciph, q1l, q1h);
	q1h = (q1h << 1) + (q1l>>(MULTUNITSIZE-1));
	q1l = (q1l << 1) ;

	ql = (ql >> (MULTUNITSIZE-2)) + (qh <<  2);
	qh >>= (MULTUNITSIZE-2);

	ql += q1l;
	qh += (ql < q1l) + q1h;

	if (mshift != MULTUNITSIZE) {
		ql = (ql >> mshift) + (qh << (MULTUNITSIZE-mshift));
		qh >>= mshift;
	} else {
		ql = qh;
		qh  = 0;
	}

/*	Prevent overflow and then wipe out the intermediate results. */
	mutemp = (qh != 0) ?  MULTUNIT_mask : ql;

	return(mutemp);
}

/* end of r3000.c */


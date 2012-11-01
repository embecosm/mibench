|	Fast assembly routines for MC68020 (Sun-3)
|	Assumptions:
|		Arguments start at sp@(0x4)
|		Return value is in d0
|		d0/d1/a0/a1 are scratch
|		P_SMUL needs MULTUNIT set to "unsigned long" in mpilib.c
|		P_DMUL replaces mp_smul and mp_dmul in mpilib.c
|
|	92.9.21 - Tsutomu Shimomura, tsutomu@ucsd.edu
|       93.5.14 - Bug in P_DMUL fixed -- now works with small bignums

	.text

|	P_SETP(p) sets the current precision to be p longwords.  No-op.
	.proc
	.globl	_P_SETP
_P_SETP:
|	movl	#L2000, a0	|%
|	jsr	mcount		|%
|	.bss			|%
|	.even			|%
|L2000:	.skip	4		|%
|	.text			|%
	rts

|	P_ADDC(*a, *b, c) performs a += b + c (carry).  Carry is returned.
	.proc
	.globl	_P_ADDC
_P_ADDC:
|	movl	#L2001, a0	|%
|	jsr	mcount		|%
|	.bss			|%
|	.even			|%
|L2001:	.skip	4		|%
|	.text			|%
	movl	sp@(0x4), a0	| claim arguments
	movl	sp@(0x8), a1
	movl	sp@(0xc), d0
	movl	d2, sp@-	| preserve d2

	movw	_global_precision, d1	| longword count
	movw	d1, d2		| save a copy

	lslw	#2, d1
	addw	d1, a0		| adjust array pointers
	addw	d1, a1

	lsrw	#1, d1		| compute initial branch offset
	andw	#0xe, d1
	negw	d1		| branch offset in d1

	lsrw	#3, d2		| 8 longwords/loop; count in d2

	asrl	#1, d0		| set X if necessary

	jmp	pc@(0x12,d1:w)
1:
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	addxl	a1@-, a0@-
	dbf	d2, 1b

	roxll	#1, d0

	movl	sp@+, d2
	rts

|	P_SUBB(*a, *b, c) performs a -= b + c (borrow).  Borrow is returned.
	.proc
	.globl	_P_SUBB
_P_SUBB:
|	movl	#L2002, a0	|%
|	jsr	mcount		|%
|	.bss			|%
|	.even			|%
|L2002:	.skip	4		|%
|	.text			|%
	movl	sp@(0x4), a0	| claim arguments
	movl	sp@(0x8), a1
	movl	sp@(0xc), d0
	movl	d2, sp@-	| preserve d2

	movw	_global_precision, d1	| longword count
	movw	d1, d2		| save a copy

	lslw	#2, d1
	addw	d1, a0		| adjust array pointers
	addw	d1, a1

	lsrw	#1, d1		| compute initial branch offset
	andw	#0xe, d1
	negw	d1		| branch offset in d1

	lsrw	#3, d2		| 8 longwords/loop; count in d2

	asrl	#1, d0		| set X if necessary

	jmp	pc@(0x12,d1:w)
1:
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	subxl	a1@-, a0@-
	dbf	d2, 1b

	roxll	#1, d0

	movl	sp@+, d2
	rts

|	P_ROTL(*a, c) performs a = (a<<1) | c (lo-bit).  Hi-bit is returned.
	.proc
	.globl	_P_ROTL
_P_ROTL:
|	movl	#L2003, a0	|%
|	jsr	mcount		|%
|	.bss			|%
|	.even			|%
|L2003:	.skip	4		|%
|	.text			|%
	movl	sp@(0x4), a0	| claim arguments
	movl	sp@(0x8), d0
	movl	d2, a1		| preserve d2

	movw	_global_precision, d1	| longword count
	movw	d1, d2		| save a copy

	lslw	#2, d1
	addw	d1, a0		| adjust array pointer

	andw	#0x1c, d1
	negw	d1		| branch offset in d1

	lsrw	#3, d2		| 8 longwords/loop; count in d2

	asrl	#1, d0		| set X if necessary

	jmp	pc@(0x22,d1:w)
1:
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	roxlw	a0@-
	dbf	d2, 1b

	roxll	#1, d0

	movl	a1, d2
	rts

|	P_SMUL(*a, *b, x) performs a += b * x.  Pointers are to the LSB.
	.proc
	.globl	_P_SMUL
_P_SMUL:
|	movl	#L2004, a0	|%
|	jsr	mcount		|%
|	.bss			|%
|	.even			|%
|L2004:	.skip	4		|%
|	.text			|%
	movl	sp@(0x4), a0	| claim arguments
	movl	sp@(0x8), a1
	movl	sp@(0xc), d1
	tstl	d1		| horrible kludge to speed multiply by 0
	beq	3f
	moveml	#0x3c00, sp@-	| d2/d3/d4/d5; Sun's optimizer is really *DUMB*
	movw	_global_precision, d5	| longword count; 0 will fail

	subqw	#2, d5		| first longword not handled in loop
	clrl	d4

	movl	a1@, d2
	mulul	d1, d3:d2	| d3 is carry
	addl	d2, a0@		| accumulate

|	tstw	d5		| This code needed if global_precision < 2
|	blt	2f		| only one longword?
1:
	movl	a0@-, d0
	addxl	d3, d0		| accumulate carry and X-bit
	movl	a1@-, d2
	mulul	d1, d3:d2	| d3 is carry
	addxl	d4, d3		| add X-bit to carry
	addl	d2, d0		| accumulate
	movl	d0, a0@
	dbf	d5, 1b
2:
	addxl	d4, d3		| add X-bit to carry
	movl	d3, a0@-	

	moveml	sp@+, #0x3c	| d2/d3/d4/d5; Sun's optimizer is really *DUMB*
3:
	rts

|	P_DMUL(*a, *b, *c) performs a = b * c.
	.proc
	.globl	_P_DMUL
_P_DMUL:
|	movl	#L2005, a0	|%
|	jsr	mcount		|%
|	.bss			|%
|	.even			|%
|L2005:	.skip	4		|%
|	.text			|%
	moveml	#0x3f38, sp@-	| d2-d7/a2-a4; Sun's optimizer is really *DUMB*
	movl	sp@(0x28), a0	| claim arguments
	movl	sp@(0x2c), a1
	movl	sp@(0x30), a2
	movw	_global_precision, d0
	subqw	#2, d0	| global_precision - 2

	movl	a0, a4	| product

	movw	d0, d7	| count for multiplicand
	movl	a1, a3	| multiplicand
	tstl	a3@+
	bne	2f
	subqw	#1, d7
1:
	clrl	a4@+
	tstl	a3@+
	dbne	d7, 1b
	addqw	#1, d7	| d7 contains effective size of the multiplicand-2
2:
	movl	d0, d6	| count for multiplier
	movl	a2, a3	| multiplier
	tstl	a3@+
	bne	2f
	subqw	#1, d6
1:
	clrl	a4@+
	tstl	a3@+
	dbne	d6, 1b
	addqw	#1, d6	| d6 contains effective size of the multiplier-2
2:

	addqw	#1, d0	| global_precision - 1
	lslw	#2, d0
	addw	d0, a1	| pointer to LSB of the multiplicand
	addw	d0, a2	| pointer to LSB of the multiplier
	addw	d0, a0
	addw	d0, a0
	addql	#4, a0	| pointer to LSB of product - KLUDGE!

|	First partial product not handled in loop
|	Assumes that the X-bit is clear from the above contortions.
	clrl	d4

	movl	a0, a3	| product
	movl	a1, a4	| multiplicand
	movl	a2@, d1	| one longword of the multiplier
	movw	d7, d5	| loop count

	movl	a4@, d2
	mulul	d1, d3:d2	| d3 is carry
	movl	d2, a3@		| store product
1:
	movl	a4@-, d2
	mulul	d1, d0:d2
	addxl	d3, d2
	movl	d0, d3
	movl	d2, a3@-
	dbf	d5, 1b

	addxl	d4, d3
	movl	d3, a3@-

| The other partial products

2:
	movl	a1, a4		| multiplicand
	movl	a2@-, d1	| another longword of the multiplier
	movw	d7, d5		| loop count

	movl	a4@, d2
	mulul	d1, d3:d2	| d3 is carry
	addl	d2, a0@-	| accumulate

	movl	a0, a3		| product

1:
	movl	a3@-, d0
	addxl	d3, d0		| accumulate carry and X-bit
	movl	a4@-, d2
	mulul	d1, d3:d2	| d3 is carry
	addxl	d4, d3		| add X-bit to carry
	addl	d2, d0		| accumulate
	movl	d0, a3@
	dbf	d5, 1b

	addxl	d4, d3		| add X-bit to carry
	movl	d3, a3@-	

	dbf	d6, 2b

	moveml	sp@+, #0x1cfc	| d2-d7/a2-a4; Sun's optimizer is really *DUMB*
	rts

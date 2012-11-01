!
! SPARC assembler primitives for rsalib.c
! written by Branko Lankester, lankeste@fwi.uva.nl	8/29/91
!
! Tested with gcc 1.39 and newer, and SunOS cc.
! NOTE: does not work with gcc 1.37 (and older?)
!
! other sources must be compiled with UNIT32 and HIGHFIRST defined
!
#ifndef SYSV
#ifdef __STDC__
#define ENTRY(name)	_##name ; _##name##:
#else
#define ENTRY(name)	_/**/name ; _/**/name/**/:
#endif
#define GLOB		_global_precision
#else /* SYSV */
#define ENTRY(name)	name ; name:
#define GLOB		global_precision
#endif

	.seg	"text"
        .proc   4
	.global ENTRY(P_SETP)
	retl
	nop


        .proc   4
	.global ENTRY(P_ADDC)
	sethi	%hi(GLOB), %o3
	ldsh	[%o3+%lo(GLOB)], %o3
	and	%o2, 1, %o2
	dec	%o3
	sll	%o3, 2, %o3
	ld	[%o0+%o3], %o4
add_loop:
	ld	[%o1+%o3], %o5
	subcc	%g0, %o2, %g0
	addxcc	%o4, %o5, %o4
	addx	%g0, %g0, %o2
	st	%o4, [%o0+%o3]
	subcc	%o3, 4, %o3
	bge,a	add_loop
	ld	[%o0+%o3], %o4
	retl
	mov	%o2, %o0		! return carry


        .proc   4
	.global ENTRY(P_SUBB)
	sethi	%hi(GLOB), %o3
	ldsh	[%o3+%lo(GLOB)], %o3
	and	%o2, 1, %o2
	dec	%o3
	sll	%o3, 2, %o3
	ld	[%o0+%o3], %o4
sub_loop:
	ld	[%o1+%o3], %o5
	subcc	%g0, %o2, %g0
	subxcc	%o4, %o5, %o4
	addx	%g0, %g0, %o2
	st	%o4, [%o0+%o3]
	subcc	%o3, 4, %o3
	bge,a	sub_loop
	ld	[%o0+%o3], %o4
	retl
	mov	%o2, %o0


        .proc   4
	.global ENTRY(P_ROTL)
	sethi	%hi(GLOB), %o3
	ldsh	[%o3+%lo(GLOB)], %o3
	and	%o1, 1, %o1
	dec	%o3
	sll	%o3, 2, %o3
	ld	[%o0+%o3], %o4
rot_loop:
	sll	%o4, 1, %o5
	or	%o5, %o1, %o5
	st	%o5, [%o0+%o3]
	srl	%o4, 31, %o1
	subcc	%o3, 4, %o3
	bge,a	rot_loop
	ld	[%o0+%o3], %o4
	retl
	mov	%o1, %o0

	.seg	"data"


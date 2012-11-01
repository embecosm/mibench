/*
**
**      80386 assembly primitives for RSA library
**      GNU gas syntax, tested with gcc v1.39
**
**      Compile with UNIT32 defined, MULTUNIT must be unsigned long
**      assemble this file with gcc -c (file should have .S suffix)
**
**      Written by Branko Lankester (lankeste@fwi.uva.nl)       11/18/91
**      Last revised: 12/01/92
**	Fixed macros to omit space before colon, 23 Jun 93
**
 */

#if defined(SYSV) || defined(__ELF__)
#define	ENTRY(name)	.align 4 ; .globl name ; name:
#else
#ifdef __STDC__
#define	ENTRY(name)	.align 4 ; .globl _##name ; _##name##:
#else
#define	ENTRY(name)	.align 4 ; .globl _/**/name ; _/**/name/**/:
#endif
#endif

.text

/*
********************* set precision ********************
*/
ENTRY (P_SETP)
        pushl %ebp
        movl %esp,%ebp
        pushl %ebx
		pushl %ecx
		pushl %edx
        movl 8(%ebp),%eax
        addl $0x1f,%eax
        shrl $5,%eax
        movl %eax,%ebx
        shrl $3,%eax
        movl %eax,prec8
        andl $7,%ebx

        movl $add_ref,%eax
        movl %eax,%ecx
        subl $add_1ref,%eax
        mul %ebx
        subl %eax,%ecx
        movl %ecx,addp
        movl $sub_ref,%ecx
        subl %eax,%ecx
        movl %ecx,subp

        movl $rot_ref,%eax
        movl %eax,%ecx
        subl $rot_1ref,%eax
        mul %ebx
        subl %eax,%ecx
        movl %ecx,rotp

        movl $mul_ref,%eax
        movl %eax,%ecx
        subl $mul_1ref,%eax
        mul %ebx
        subl %eax,%ecx
        movl %ecx,mulp

		popl %edx
		popl %ecx
        popl %ebx
        leave
        ret



/*
********************* mpi add with carry ********************
*/

#define ADDU    lodsl ; adcl %eax,(%ebx,%esi)

ENTRY (P_ADDC)
        pushl %ebp
        movl %esp,%ebp
        pushl %ebx
		pushl %ecx
        pushl %esi
        pushl %edi
        movl 12(%ebp),%esi
        movl 8(%ebp),%ebx
        subl %esi,%ebx
        subl $4,%ebx
        cld
        movl 16(%ebp),%eax
        movl prec8,%ecx
        orl %ecx,%ecx
        rcrl $1,%eax	/* set the carry flag */
		jz add_units	/* z-flag set by orl %ecx,%ecx */
add_8u:
        ADDU ; ADDU ; ADDU ; ADDU ; ADDU ; ADDU ; ADDU ; ADDU
        loop add_8u
add_units:
        jmp *addp
        ADDU ; ADDU ; ADDU ; ADDU ; ADDU ; ADDU
add_1ref:                       /* label to compute size of codes */
        ADDU
add_ref:
        rcll $1,%eax
        andl $1,%eax

        popl %edi
        popl %esi
		popl %ecx
        popl %ebx
        leave
        ret


/*
********************* mpi subtract with borrow ********************
*/

#define SUBU    lodsl ; sbbl %eax,(%ebx,%esi)

ENTRY (P_SUBB)
        pushl %ebp
        movl %esp,%ebp
        pushl %ebx
		pushl %ecx
        pushl %esi
        pushl %edi
        movl 12(%ebp),%esi
        movl 8(%ebp),%ebx
        subl %esi,%ebx
        subl $4,%ebx
        cld
        movl 16(%ebp),%eax
        movl prec8,%ecx
        orl %ecx,%ecx
        rcrl $1,%eax
		jz sub_units
sub_8u:
        SUBU ; SUBU ; SUBU ; SUBU ; SUBU ; SUBU ; SUBU ; SUBU
        loop sub_8u
sub_units:
        jmp *subp
        SUBU ; SUBU ; SUBU ; SUBU ; SUBU ; SUBU ; SUBU
sub_ref:
        rcll $1,%eax
        andl $1,%eax

        popl %edi
        popl %esi
		popl %ecx
        popl %ebx
        leave
        ret



/*
********************* mpi rotate left ********************
*/

#define ROTU    rcll $1,(%ebx,%esi,4) ; incl %esi

ENTRY (P_ROTL)
        pushl %ebp
        movl %esp,%ebp
        pushl %ebx
		pushl %ecx
        pushl %esi
        movl 8(%ebp),%ebx
        movl 12(%ebp),%eax
        xorl %esi,%esi
        movl prec8,%ecx
        orl %ecx,%ecx
        rcrl $1,%eax	/* set the carry flag */
		jz rot_units	/* z-flag set by orl %ecx,%ecx */
rot_8u:
        rcll $1,(%ebx)
        rcll $1,4(%ebx)
        rcll $1,8(%ebx)
        rcll $1,12(%ebx)
        rcll $1,16(%ebx)
        rcll $1,20(%ebx)
        rcll $1,24(%ebx)
        rcll $1,28(%ebx)
        leal 32(%ebx),%ebx
        loop rot_8u
rot_units:
        jmp *rotp
        ROTU ; ROTU ; ROTU ; ROTU ; ROTU ; ROTU ; ROTU
rot_1ref:
        ROTU
rot_ref:
        rcll $1,%eax
        andl $1,%eax

        popl %esi
		popl %ecx
        popl %ebx
        leave
        ret

/*
********************* mpi multiply ********************
*/
#define MULU    \
        lodsl ; \
        mull %ebp ; \
        addl %ebx,%eax ; \
        adcl $0,%edx ; \
        addl (%edi),%eax ; \
        adcl $0,%edx ; \
        movl %edx,%ebx ; \
        stosl

ENTRY (P_SMULA)
        pushl %ebp
        movl %esp,%ebp
        pushl %ebx
		pushl %ecx
		pushl %edx
        pushl %esi
        pushl %edi

        xorl %ebx,%ebx
        movl prec8,%ecx
        movl 8(%ebp),%edi
        movl 12(%ebp),%esi
        movl 16(%ebp),%ebp
        cld
        orl %ecx,%ecx
        jz mul_units
mul_8u:
        MULU ; MULU ; MULU ; MULU ; MULU ; MULU ; MULU ; MULU
        decl %ecx
        jnz mul_8u      /* offset too big for loop */
mul_units:
        jmp *mulp
        MULU ; MULU ; MULU ; MULU ; MULU ; MULU
mul_1ref:
        MULU
mul_ref:
        addl %ebx,(%edi)

        popl %edi
        popl %esi
		popl %edx
		popl %ecx
        popl %ebx
        popl %ebp
        ret


.lcomm _reciph,4
.lcomm _recipl,4
.lcomm _mshift,4

ENTRY (p_setrecip)
		movl 4(%esp),%eax
		movl %eax,_reciph
		movl 8(%esp),%eax
		movl %eax,_recipl
		movl 12(%esp),%eax
		movl %eax,_mshift
		ret


ENTRY (p_quo_digit)
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi

	movl 20(%esp),%esi       /* dividend */
	movl -8(%esi),%eax   /* dividend[-2] */
	notl %eax
	mull _reciph
	addl _reciph,%eax
	adcl $0,%edx
	movl %eax,%ebx
	movl %edx,%edi          /* di:bx = q1 */

	movl -4(%esi),%eax   /* dividend[-1] */
	notl %eax
	mull _recipl
	incl %edx             /* dx:ax = q2 */

	movl %edx,%ebp
	andl %edi,%ebp
	andl $1,%ebp           /* bp = lsb_factor */

	addl %ebx,%eax
	adcl %edx,%edi
	rcrl $1,%edi           /* di = MS word of q0 */

	movl -4(%esi),%eax   /* dividend [-1] */
	notl %eax
	mull _reciph
	movl %eax,%ebx
	movl %edx,%ecx          /* cx:bx = q1 */

	movl (%esi),%eax     /* dividend[0] */
	notl %eax
	mull _recipl         /* dx:ax = q2 */
	xorl %ebx,%eax
	andl %eax,%ebp          /* lsb correction */
	xorl %ebx,%eax          /* restore ax */

	addl %ebx,%eax
	adcl %ecx,%edx
	rcrl $1,%edx
	rcrl $1,%eax           /* dx:ax = q */

	addl %edi,%eax          /* + scaled q0 */
	adcl $0,%edx
	addl %ebp,%eax          /* + lsb correction */
	adcl $0,%edx           /* q */

	shll $1,%eax
	rcll $1,%edx
	rcll $1,%eax
	rcll $1,%edx
	rcll $1,%eax
	andl $3,%eax
	movl %eax,%ecx
	movl %edx,%ebx          /* bx:cx = q >> 14 */

	movl (%esi),%eax     /* dividend[0] */
	notl %eax
	mull _reciph
	shll $1,%eax
	rcll $1,%edx
	addl %ebx,%eax
	adcl %ecx,%edx          /* q */

	movl _mshift,%ecx
	cmpl $32,_mshift
	je L2
#if 0
	shrl %cl,%eax
	movl %edx,%ebx
	shrl %cl,%edx
	negl %ecx
	addl $32,%ecx
	shll %cl,%ebx
	addl %ebx,%eax          /* dx:ax = q >> mshift */
#else
#if defined(SYSV) || defined(__ELF__)
	shrdl %edx,%eax
#else
	shrdl %cl,%edx,%eax
#endif
	shrl %cl,%edx
#endif

	orl %edx,%edx
	je L1
	movl $-1,%eax
	jmp L1
L2:
	xchgl %edx,%eax
L1:
	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret

.lcomm  prec8,4
.lcomm  addp,4
.lcomm  subp,4
.lcomm  rotp,4
.lcomm  mulp,4


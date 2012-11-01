;-------------------------------------------------------------------------
; idea68k.a 
;
; 68000 Assembler version of idea cipher, direct translation from c code
; from PGP.
;
; Author: Risto Paasivirta, paasivir@jyu.fi.
;

		section text,code

		xdef	_asm_mul,_asm_inv
		xdef	_asm_cipher_idea

; key schedule block

ROUNDS		equ	8

Z0		equ	0
Z1		equ	(ROUNDS+1)*2
Z2		equ	(ROUNDS+1)*4
Z3		equ	(ROUNDS+1)*6
Z4		equ	(ROUNDS+1)*8
Z5		equ	(ROUNDS+1)*10

ZSIZE		equ	(ROUNDS+1)*12

KSSIZE		equ	ZSIZE*2

;-------------------------------------------------------------------------
;
; idmul da,db -- db = da * db mod 65537, d0 = scratch (da may be d0)
;

idmul		macro
		tst.w	\2
		bne.b	idmul1.\@

		moveq	#1,\2
		sub.w	\1,\2
		bra.b	idmul3.\@

idmul1.\@	tst.w	\1
		bne.b	idmul2.\@
		moveq	#1,d0
		sub.w	\2,d0
		move.w	d0,\2
		bra.b	idmul3.\@

idmul2.\@	mulu.w	\1,\2
		move.l	\2,d0
		swap	d0
		sub.w	d0,\2
		bcc.b	idmul3.\@
		addq.w	#1,\2
idmul3.\@
		endm

;-------------------------------------------------------------------------
; idea_cip (a0=inblock,a1=outblock,a2=keyshedule) (d0-d7/a3 scratch)
;
;
;

idea_cip	movem.w (a0),d1-d4
		moveq	#0,d7

idea_cip_loop	lea	0(a2,d7.w),a3
		idmul	(a3),d1
		idmul	Z3(a3),d4
		add.w	Z1(a3),d2
		add.w	Z2(a3),d3
		move.w	d1,d6
		eor.w	d3,d6
		idmul	Z4(a3),d6
		move.w	d4,d5
		eor.w	d2,d5
		add.w	d6,d5
		idmul	Z5(a3),d5
		add.w	d5,d6
		eor.w	d5,d1
		eor.w	d6,d4
		eor.w	d6,d2
		eor.w	d5,d3
		exg	d2,d3
		addq.w	#2,d7
		cmp.w	#ROUNDS*2,d7
		bcs	idea_cip_loop

		lea	0(a2,d7.w),a3
		idmul	(a3),d1
		idmul	Z3(a3),d4
		add.w	Z1(a3),d3
		add.w	Z2(a3),d2
		exg	d2,d3
		movem.w	d1-d4,(a1)
		rts

;-------------------------------------------------------------------------
; _asm_cipher_idea(word16 *in,word16 *out,word16 *ks)
;
;
;

_asm_cipher_idea
		movem.l	a2-a3/d2-d7,-(sp)
		movem.l	36(sp),a0-a2
		bsr	idea_cip
		movem.l	(sp)+,a2-a3/d2-d7
		rts

;-------------------------------------------------------------------------
; word16 _asm_mul(word16, word16);
;
;

_asm_mul	move.w	6(sp),d1
		idmul	10(sp),d1
		moveq	#0,d0
		move.w	d1,d0
		rts

;-------------------------------------------------------------------------
; d0:16 = inv(d0) 
;

_asm_inv	move.w	6(sp),d0

inv		cmp.w	#2,d0		; inv(0)=0,inv(1)=1
		bcs.b	1$

		cmp.w	#3,d0
		bcc.b	2$

		move.w	#32769,d0	; inv(2)
1$		rts

2$		movem.l	d1-d7,-(sp)
		move.l	#$10001,d1	; d1 = n1
		moveq	#1,d2		; d2 = b2
		moveq	#0,d3		; d3 = b1		

inv_loop	divu.w	d0,d1
		move.l	d1,d4
		swap	d4		; r = d4
		tst.w	d4
		beq.b	inv_done

		move.w	d2,d5
		muls.w	d1,d5
		exg	d3,d2
		sub.l	d5,d2
		moveq	#0,d1
		move.w 	d0,d1
		move.w	d4,d0
		bra.b	inv_loop

inv_done	tst.l	d2
		bpl.b	1$

		move.l	#$10001,d0
		add.l	d2,d0
		bra.b	2$

1$		move.l	d2,d0

2$		movem.l	(sp)+,d1-d7
		rts

;-------------------------------------------------------------------------

		end


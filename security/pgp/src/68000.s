; This is the assembler source code for amiga 68000
; It assumes Lattice-C is used. (v5.04 or higher)
; Written by S. Fishman

    SECTION text

	XREF	_global_precision
	XDEF	_P_ADDC		;make the linker know we're here

_P_ADDC:	link	a6,#0
	movem.l	d4-d5/a0-a1,-(a7)	
	move.l	8(a6),a0	
	move.l	12(a6),a1	
	move.l	16(a6),d4
	sne	d4		
	moveq.l	#0,d5
	move.w	_global_precision,d5
	asl.l	#2,d5		
	add.l	d5,a0	
	add.l	d5,a1
	asr.l	#2,d5		
	sub.l	#1,d5	
	asr.b	#1,d4	
add_loop:	addx.l	-(a1),-(a0)	
	dbra	d5,add_loop
	scs	d0		
	ext.w	d0
	ext.l	d0		
	movem.l	(a7)+,d4-d5/a0-a1	
	unlk	a6		;get A6 & A7 back
	rts

	XDEF	_P_SUBB		

_P_SUBB:	link	a6,#0
	movem.l	d4-d5/a0-a1,-(a7)	
	move.l	8(a6),a0	
	move.l	12(a6),a1	
	move.l	16(a6),d4
	sne	d4		;fix old borrow bits
	moveq.l	#0,d5
	move.w	_global_precision,d5
	asl.l	#2,d5	
	add.l	d5,a0
	add.l	d5,a1		
	asr.l	#2,d5	
	sub.l	#1,d5
	asr.b	#1,d4		
sub_loop:	subx.l	-(a1),-(a0)	;come'n, let's do it !!!
	dbra	d5,sub_loop
	scs	d0		
	ext.w	d0
	ext.l	d0		
	movem.l	(a7)+,d4-d5/a0-a1	
	unlk	a6	
	rts

	XDEF	_P_ROTL		

_P_ROTL:	link	a6,#0	
	movem.l	d4-d5/a0,-(a7)
	move.l	8(a6),a0	
	move.l	12(a6),d4
	moveq.l	#0,d5
	move.w	_global_precision,d5
	asl.l	#1,d5		
				
	sub.l	#2,d5	
	asl	(a0)+
	scs	d0		
	ext.w	d0
    ext.l	d0		
rotl_loop:	asl	(a0)+
	bcc	rotl_no_carry
	bset	#0,-3(a0)	
rotl_no_carry:	dbra	d5,rotl_loop	
	tst.l	d4		
	beq	rotl_no_old_carry
	bset	#0,-1(a0)	
rotl_no_old_carry:
	movem.l	(a7)+,d4-d5/a0	
	unlk	a6		
	rts

   XDEF     _P_SETP    

_P_SETP: rts
	END


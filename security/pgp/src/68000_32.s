;FPRIMS32.S - rsalib assembler primitives for MC680x0
;             (Pure-C/Atari ST version, 32-bit units)
;
;Written by Stephan Baucke 19-Oct-91
;Assembler: Pure-PASM
;
;On systems with 68020 or higher it might be faster to use 32-Bit units
;instead of 16 Bits (I didn't yet test this, but there is no improvement
;on a plain 68000).
;
;Note that the function P_SETP of the Intel primitives is not needed here.
;`set_precision' has to be defined just like in `PORTABLE' mode in rsalib.h

            IMPORT global_precision
            EXPORT P_ADDC, P_SUBB, P_ROTL, P_SETP, P_SMUL

            TEXT

;boolean P_ADDC(unitptr r1, unitptr r2, boolean carry);
; /* multiprecision add with carry r2 to r1, result in r1 */
;Parameters: A0.l: r1, A1.l: r2, D0.b: carry
;Result:     D0.b: new carry
;Modifies: D0-D2/A0-A1

P_ADDC:     move.b  d0,d1       ;boolean carry
            sne     d1          ;set bit 0 to carry
            move.w  global_precision,d2 ;# of units
            move.w  d2,d0       ;make copy
            lsl.w   #2,d2       ;calc # of bytes (4 per unit)
            add.w   d2,a0       ;point r1 to least significant unit
            add.w   d2,a1       ;point r2 to least significant unit
            lsr.w   #1,d2       ;divide back (now 2 * units)
            and.w   #2*7,d2     ;yields 2 * (units % 8)
            neg.w   d2          ;negative offset (for 2-Byte instructions)
            lsr.w   #3,d0       ;units / 8
            beq.s   .onebyone   ;skip loop if zero count

            subq.w  #1,d0       ;one off (dbf counter)
            lsr.b   d1          ;set X if carry
.loop:      REPT    8               ;8 units per run
              addx.l -(a1),-(a0)
            ENDM
            dbf     d0,.loop
            jmp     .base(pc,d2.w)  ;do remaining units

.onebyone:  lsr.b   d1              ;set X if carry
            jmp     .base(pc,d2.w)  ;do remaining units

            REPT    7               ;max 7 units remaining
              addx.l -(a1),-(a0)    ;(2-byte instruction)
            ENDM
.base:      scs     d0              ;set returned carry
            rts

;boolean P_SUBB(unitptr r1, unitptr r2, boolean borrow);
; /* multiprecision subtract with borrow, r2 from r1, result in r1 */
;Parameters: A0.l: r1, A1.l: r2, D0.b: borrow
;Result:     D0.b: new borrow
;Modifies: D0-D2/A0/A1

P_SUBB:     move.b  d0,d1       ;boolean carry
            sne     d1          ;set bit 0 to carry
            move.w  global_precision,d2 ;# of units
            move.w  d2,d0       ;make copy
            lsl.w   #2,d2       ;calc # of bytes (4 per unit)
            add.w   d2,a0       ;point r1 to least significant unit
            add.w   d2,a1       ;point r2 to least significant unit
            lsr.w   #1,d2       ;divide back (now 2 * units)
            and.w   #2*7,d2     ;yields 2 * (units % 8)
            neg.w   d2          ;negative offset (for 2-byte instructions)
            lsr.w   #3,d0       ;units / 8
            beq.s   .onebyone   ;skip loop if zero count

            subq.w  #1,d0       ;one off (dbf counter)
            lsr.b   d1          ;set X if carry
.loop:      REPT    8               ;8 units per run
              subx.l -(a1),-(a0)
            ENDM
            dbf     d0,.loop
            jmp     .base(pc,d2.w)  ;do remaining units

.onebyone:  lsr.b   d1              ;set X if carry
            jmp     .base(pc,d2.w)  ;do remaining units

            REPT    7               ;max 7 units remaining
              subx.l -(a1),-(a0)    ;(2-byte instruction)
            ENDM
.base:      scs     d0              ;set returned carry
            rts

;boolean P_ROTL(unitptr r1, boolean carry);
; /* multiprecision rotate left 1 bit with carry, result in r1. */
;Parameters: A0.l: r1, D0.b: carry
;Result:     D0.b: new carry
;Modifies: D0-D2/A0

P_ROTL:     move.b  d0,d1       ;boolean carry
            sne     d1          ;set bit 0 to carry
            move.w  global_precision,d2 ;# of units
            move.w  d2,d0       ;make copy
            lsl.w   #2,d2       ;calc # of bytes (4 per unit)
            add.w   d2,a0       ;point r1 to least significant unit
            and.w   #4*7,d2     ;yields 4 * (units % 8)
            neg.w   d2          ;negative offset (for 4-byte instructions)
            lsr.w   #3,d0       ;units / 8
            beq.s   .onebyone   ;skip loop if zero count

            subq.w  #1,d0       ;one off (dbf counter)
            lsr.b   d1          ;set X if carry
.loop:      REPT    8           ;8 units per run
              roxl.w -(a0)      ;(roxl.l <ea> is not allowed on the 68000)
              roxl.w -(a0)
            ENDM
            dbf     d0,.loop
            jmp     .base(pc,d2.w)  ;do remaining units

.onebyone:  lsr.b   d1              ;set X if carry
            jmp     .base(pc,d2.w)  ;do remaining units

            REPT    7               ;max 7 units remaining
              roxl.w -(a0)          ;(4 bytes instructions per unit)
              roxl.w -(a0)
            ENDM
.base:      scs     d0              ;set returned carry
            rts


;void P_SETP(short nbits);
; /* sets working precision to specified number of bits. */
; /* only to minimize portation differences              */
;Parameters: --
;Result:     --

P_SETP:		rts

;void P_SMUL(MULTUNIT *prod, MULTUNIT *multiplicand, MULTUNIT multiplier)
; /* multiprecision multiply */
;Parameters: A0.l: prod, A1.l: multiplicand, D0.w: multiplier
;Modifies: D0-D3/A0-A1
;Result:     --

; Copied from 3B168000.S (GG)
P_SMUL:     ;mov.l   4(sp),a0	;# fetch first argument: prod
	;PureC already puts arguments there (GG)
	    ;mov.l   8(sp),a1	;# fetch second argument: multiplicand
	    ;mov.l   12(sp),d0	;# fetch third argument: multiplier
	    ;mov.l   d2,-(sp)	;# save d2 -- not a Unix-PC scratch reg
;	(OK for PureC -- GG)
	    move.l   d3,-(sp)  ;# save d3 -- not a Unix-PC scratch reg
	    move.l   d4,-(sp)  ;# save d4 -- not a Unix-PC scratch reg
	clr.l	d2		;# clear the carry register
	clr.l	d4		;# clear upper half of temp reg for prod
	adda.w	#2,a0		;# position prod to 1 beyond LSB
	adda.w	#2,a1		;# position multiplicand to 1 beyond LSB
	move.w	global_precision,d3  ;# fetch size of multiplicand
	add.w	d3,d3	; GG: convert 32bit to 16bit units
	bra	ddbf1

dloop:	move.w	-(a1),d1	;# fetch multiplicand
	mulu.w	d0,d1		;# multiply by multiplier
	move.w	-(a0),d4	;# fetch prod
	add.l	d4,d1		;# add in prod
	add.l	d2,d1		;# add in carry
	move.w	d1,(a0)	;# store result back to prod
	swap.w	d1		;# fetch carry info (upper 16 bits of mult)
	move.w	d1,d2		;# and move it into carry
ddbf1:	dbf	d3,dloop

	move.w	d2,-(a0)	;# store carry
	move.l	(sp)+,d4
	move.l	(sp)+,d3
;	move.l	(sp)+,d2
	rts

	END

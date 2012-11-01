# 3B1 Assembler - primitives for multi-precision math on the MC68010
#
# Written by Rob Stampfli  19-Oct-92 for 3B1
# Assembler: 3B1 native assembler
#
# Note that the function P_SETP of the Intel primitives is not used.
# `set_precision' has to be defined just like in `PORTABLE' mode.

	    global global_precision
	    global P_ADDC
	    global P_SUBB
	    global P_ROTL
	    global P_SETP
	    global P_SMUL

	    text

#boolean P_ADDC(unitptr r1, unitptr r2, boolean carry);
# /* multiprecision add with carry r2 to r1, result in r1 */
#Parameters: A0.l: r1, A1.l: r2, D0.b: carry
#Result:     D0.b: new carry
#Modifies: D0-D2/A0-A1

P_ADDC:     mov.l   4(%sp),%a0	# fetch first argument: r1
	    mov.l   8(%sp),%a1	# fetch second argument: r2
	    mov.l   12(%sp),%d1	# fetch third argument: carry
	    mov.l   %d2,-(%sp)	# save d2 -- not a Unix-PC scratch reg
	    mov.w   global_precision,%d2 # fetch # of 16 bit units
	    mov.w   %d2,%d0	# copy units
	    lsl.w   &1,%d0	# convert units to bytes (1 unit = 2 bytes)
	    add.w   %d0,%a0	# point r1 to 1 past least significant unit
	    add.w   %d0,%a1	# point r2 to 1 past least significant unit
	    lsr.w   &5,%d0	# conv bytes to (units/16); # times thru loop1
	    and.w   &15,%d2	# = (units%16); # times thru loop2
	    lsr.b   &1,%d1	# set X-bit as specified by carry arg
	    bra     adbf1
aloop1:	    #REPT    8		# 16 units per loop
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
	    addx.l -(%a1),-(%a0)
adbf1:	    dbf     %d0,aloop1
	    bra adbf2
aloop2:	    addx.w -(%a1),-(%a0)
adbf2:	    dbf     %d2,aloop2
	    scs     %d0		    #set returned carry
	    mov.l   (%sp)+,%d2	    # restore d2
	    rts

#boolean P_SUBB(unitptr r1, unitptr r2, boolean borrow);
# /* multiprecision subtract with borrow, r2 from r1, result in r1 */
#Parameters: A0.l: r1, A1.l: r2, D0.b: borrow
#Result:     D0.b: new borrow
#Modifies: D0-D2/A0/A1

P_SUBB:     mov.l   4(%sp),%a0	# fetch first argument: r1
	    mov.l   8(%sp),%a1	# fetch second argument: r2
	    mov.l   12(%sp),%d1	# fetch third argument: carry
	    mov.l   %d2,-(%sp)	# save d2 -- not a Unix-PC scratch reg
	    mov.w   global_precision,%d2 # fetch # of 16 bit units
	    mov.w   %d2,%d0	# copy units
	    lsl.w   &1,%d0	# convert units to bytes (1 unit = 2 bytes)
	    add.w   %d0,%a0	# point r1 to 1 past least significant unit
	    add.w   %d0,%a1	# point r2 to 1 past least significant unit
	    lsr.w   &5,%d0	# conv bytes to (units/16); # times thru loop1
	    and.w   &15,%d2	# = (units%16); # times thru loop2
	    lsr.b   &1,%d1	# set X-bit as specified by carry arg
	    bra     bdbf1
bloop1:	    #REPT    8		# 16 units per loop
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
	    subx.l  -(%a1),-(%a0)
bdbf1:	    dbf     %d0,bloop1
	    bra bdbf2
bloop2:	    subx.w -(%a1),-(%a0)
bdbf2:	    dbf     %d2,bloop2
	    scs     %d0		    # set returned carry
	    mov.l   (%sp)+,%d2	    # restore d2
	    rts

#boolean P_ROTL(unitptr r1, boolean carry);
# /* multiprecision rotate left 1 bit with carry, result in r1. */
#Parameters: A0.l: r1, D0.b: carry
#Result:     D0.b: new carry
#Modifies: D0-D2/A0

P_ROTL:     mov.l   4(%sp),%a0	# fetch first argument: r1
	    mov.l   8(%sp),%d1	# fetch second argument: carry
	    mov.l   %d2,-(%sp)	# save d2 -- not a Unix-PC scratch reg
	    mov.w   global_precision,%d2 # fetch # of 16 bit units
	    mov.w   %d2,%d0	# copy units
	    lsl.w   &1,%d0	# convert units to bytes (1 unit = 2 bytes)
	    add.w   %d0,%a0	# point r1 to 1 past least significant unit
	    lsr.w   &5,%d0	# conv bytes to (units/16); # times thru loop1
	    and.w   &15,%d2	# = (units%16); # times thru loop2
	    lsr.b   &1,%d1	# set X-bit as specified by carry arg
	    bra     cdbf1
cloop1:	    #REPT    16		# note roxl.l not valid on 68010
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
	    roxl.w  &1,-(%a0)
cdbf1:	    dbf     %d0,cloop1
	    bra     cdbf2
cloop2:	    roxl.w  &1,-(%a0)
cdbf2:	    dbf     %d2,cloop2
	    scs     %d0		    # set returned carry
	    mov.l   (%sp)+,%d2	    # restore d2
	    rts

#void P_SETP(short nbits);
# /* sets working precision to specified number of bits. */
# /* only to minimize portation differences		 */
#Parameters: --
#Result:     --

P_SETP:	    rts

#void P_SMUL(MULTUNIT *prod, MULTUNIT *multiplicand, MULTUNIT multiplier)
# /* multiprecision multiply */
#Parameters: A0.l: prod, A1.l: multiplicand, D0.w: multiplier
#Modifies: D0-D4/A0-A1
#Result:     --
#Note: prod and multiplicand have already been adjusted to point to LSB
# prior to making the call.

P_SMUL:     mov.l   4(%sp),%a0	# fetch first argument: prod
	    mov.l   8(%sp),%a1	# fetch second argument: multiplicand
	    mov.l   12(%sp),%d0	# fetch third argument: multiplier
	    mov.l   %d2,-(%sp)	# save d2 -- not a Unix-PC scratch reg
	    mov.l   %d3,-(%sp)  # save d3 -- not a Unix-PC scratch reg
	    mov.l   %d4,-(%sp)  # save d4 -- not a Unix-PC scratch reg
	clr.l	%d2		# clear the carry register
	clr.l	%d4		# clear upper half of temp reg for prod
	add.w	&2,%a0		# position prod to 1 beyond LSB
	add.w	&2,%a1		# position multiplicand to 1 beyond LSB
	mov.w	global_precision,%d3  # fetch size of multiplicand
	bra	ddbf1

dloop:	mov.w	-(%a1),%d1	# fetch multiplicand
	mulu.w	%d0,%d1		# multiply by multiplier
	mov.w	-(%a0),%d4	# fetch prod
	add.l	%d4,%d1		# add in prod
	add.l	%d2,%d1		# add in carry
	mov.w	%d1,(%a0)	# store result back to prod
	swap.w	%d1		# fetch carry info (upper 16 bits of mult)
	mov.w	%d1,%d2		# and move it into carry
ddbf1:	dbf	%d3,dloop

	mov.w	%d2,-(%a0)	# store carry
	mov.l	(%sp)+,%d4
	mov.l	(%sp)+,%d3
	mov.l	(%sp)+,%d2
	rts

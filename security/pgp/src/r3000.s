	.file	1 "r3000.c"
	.set	nobopt

 # GNU C 2.3.1 [AL 1.1, MM 28] DECstation running ultrix compiled by GNU C

 # Cc1 defaults:

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -mgpopt -quiet -dumpbase -O -o

gcc2_compiled.:
	.text
	.align	2
	.globl	p_addc
	.align	2
	.globl	p_subb
	.align	2
	.globl	p_rotl
	.align	2
	.globl	p_setp
	.align	2
	.globl	p_smula
	.align	2
	.globl	p_setrecip
	.align	2
	.globl	p_quo_digit

	.lcomm	mshift,8

	.lcomm	reciph,8

	.lcomm	recipl,8

	.extern	global_precision, 2

	.text

	.loc	1 99
	.ent	p_addc
p_addc:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lhu	$9,global_precision
	li	$2,-1			# 0xffffffff
	andi	$8,$9,0x0003
	subu	$8,$8,1
	.set	noreorder
	.set	nomacro
	beq	$8,$2,$L3
	andi	$6,$6,0x00ff
	.set	macro
	.set	reorder

	li	$10,-1			# 0xffffffff
$L6:
	beq	$6,$0,$L4
	lw	$3,0($4)
	lw	$2,0($5)
	#nop
	addu	$7,$3,$2
	addu	$7,$7,1
	nor	$3,$0,$3
	sltu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L5
	xori	$6,$2,0x0001
	.set	macro
	.set	reorder

$L4:
	lw	$3,0($4)
	lw	$2,0($5)
	#nop
	addu	$7,$3,$2
	sltu	$6,$7,$3
$L5:
	addu	$5,$5,4
	sw	$7,0($4)
	subu	$8,$8,1
	.set	noreorder
	.set	nomacro
	bne	$8,$10,$L6
	addu	$4,$4,4
	.set	macro
	.set	reorder

$L3:
	sll	$2,$9,16
	sra	$8,$2,18
	subu	$8,$8,1
	li	$2,-1			# 0xffffffff
	beq	$8,$2,$L8
	li	$9,-1			# 0xffffffff
$L17:
	beq	$6,$0,$L9
	lw	$3,0($4)
	lw	$2,0($5)
	#nop
	addu	$7,$3,$2
	addu	$7,$7,1
	nor	$3,$0,$3
	sltu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L10
	xori	$6,$2,0x0001
	.set	macro
	.set	reorder

$L9:
	lw	$3,0($4)
	lw	$2,0($5)
	#nop
	addu	$7,$3,$2
	sltu	$6,$7,$3
$L10:
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L11
	sw	$7,0($4)
	.set	macro
	.set	reorder

	lw	$3,4($4)
	lw	$2,4($5)
	#nop
	addu	$7,$3,$2
	addu	$7,$7,1
	nor	$3,$0,$3
	sltu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L12
	xori	$6,$2,0x0001
	.set	macro
	.set	reorder

$L11:
	lw	$3,4($4)
	lw	$2,4($5)
	#nop
	addu	$7,$3,$2
	sltu	$6,$7,$3
$L12:
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L13
	sw	$7,4($4)
	.set	macro
	.set	reorder

	lw	$3,8($4)
	lw	$2,8($5)
	#nop
	addu	$7,$3,$2
	addu	$7,$7,1
	nor	$3,$0,$3
	sltu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L14
	xori	$6,$2,0x0001
	.set	macro
	.set	reorder

$L13:
	lw	$3,8($4)
	lw	$2,8($5)
	#nop
	addu	$7,$3,$2
	sltu	$6,$7,$3
$L14:
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L15
	sw	$7,8($4)
	.set	macro
	.set	reorder

	lw	$3,12($4)
	lw	$2,12($5)
	#nop
	addu	$7,$3,$2
	addu	$7,$7,1
	nor	$3,$0,$3
	sltu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L16
	xori	$6,$2,0x0001
	.set	macro
	.set	reorder

$L15:
	lw	$3,12($4)
	lw	$2,12($5)
	#nop
	addu	$7,$3,$2
	sltu	$6,$7,$3
$L16:
	sw	$7,12($4)
	addu	$4,$4,16
	subu	$8,$8,1
	.set	noreorder
	.set	nomacro
	bne	$8,$9,$L17
	addu	$5,$5,16
	.set	macro
	.set	reorder

$L8:
	.set	noreorder
	.set	nomacro
	j	$31
	andi	$2,$6,0x00ff
	.set	macro
	.set	reorder

	.end	p_addc

	.loc	1 153
	.ent	p_subb
p_subb:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$8,$4
	lhu	$10,global_precision
	li	$2,-1			# 0xffffffff
	andi	$9,$10,0x0003
	subu	$9,$9,1
	.set	noreorder
	.set	nomacro
	beq	$9,$2,$L20
	andi	$6,$6,0x00ff
	.set	macro
	.set	reorder

	li	$11,-1			# 0xffffffff
$L23:
	beq	$6,$0,$L21
	lw	$4,0($8)
	lw	$3,0($5)
	#nop
	subu	$2,$4,$3
	subu	$7,$2,$6
	sltu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	j	$L22
	xori	$6,$3,0x0001
	.set	macro
	.set	reorder

$L21:
	lw	$3,0($8)
	lw	$2,0($5)
	#nop
	subu	$7,$3,$2
	sltu	$6,$3,$2
$L22:
	addu	$5,$5,4
	sw	$7,0($8)
	subu	$9,$9,1
	.set	noreorder
	.set	nomacro
	bne	$9,$11,$L23
	addu	$8,$8,4
	.set	macro
	.set	reorder

$L20:
	sll	$2,$10,16
	sra	$9,$2,18
	subu	$9,$9,1
	li	$2,-1			# 0xffffffff
	beq	$9,$2,$L25
	li	$10,-1			# 0xffffffff
$L34:
	beq	$6,$0,$L26
	lw	$4,0($8)
	lw	$3,0($5)
	#nop
	subu	$2,$4,$3
	subu	$7,$2,$6
	sltu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	j	$L27
	xori	$6,$3,0x0001
	.set	macro
	.set	reorder

$L26:
	lw	$3,0($8)
	lw	$2,0($5)
	#nop
	subu	$7,$3,$2
	sltu	$6,$3,$2
$L27:
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L28
	sw	$7,0($8)
	.set	macro
	.set	reorder

	lw	$4,4($8)
	lw	$3,4($5)
	#nop
	subu	$2,$4,$3
	subu	$7,$2,$6
	sltu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	j	$L29
	xori	$6,$3,0x0001
	.set	macro
	.set	reorder

$L28:
	lw	$3,4($8)
	lw	$2,4($5)
	#nop
	subu	$7,$3,$2
	sltu	$6,$3,$2
$L29:
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L30
	sw	$7,4($8)
	.set	macro
	.set	reorder

	lw	$4,8($8)
	lw	$3,8($5)
	#nop
	subu	$2,$4,$3
	subu	$7,$2,$6
	sltu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	j	$L31
	xori	$6,$3,0x0001
	.set	macro
	.set	reorder

$L30:
	lw	$3,8($8)
	lw	$2,8($5)
	#nop
	subu	$7,$3,$2
	sltu	$6,$3,$2
$L31:
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L32
	sw	$7,8($8)
	.set	macro
	.set	reorder

	lw	$4,12($8)
	lw	$3,12($5)
	#nop
	subu	$2,$4,$3
	subu	$7,$2,$6
	sltu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	j	$L33
	xori	$6,$3,0x0001
	.set	macro
	.set	reorder

$L32:
	lw	$3,12($8)
	lw	$2,12($5)
	#nop
	subu	$7,$3,$2
	sltu	$6,$3,$2
$L33:
	sw	$7,12($8)
	addu	$8,$8,16
	subu	$9,$9,1
	.set	noreorder
	.set	nomacro
	bne	$9,$10,$L34
	addu	$5,$5,16
	.set	macro
	.set	reorder

$L25:
	.set	noreorder
	.set	nomacro
	j	$31
	andi	$2,$6,0x00ff
	.set	macro
	.set	reorder

	.end	p_subb

	.loc	1 211
	.ent	p_rotl
p_rotl:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$6,$4
	lh	$4,global_precision
	li	$2,-1			# 0xffffffff
	andi	$7,$4,0x0003
	subu	$7,$7,1
	.set	noreorder
	.set	nomacro
	beq	$7,$2,$L37
	andi	$5,$5,0x00ff
	.set	macro
	.set	reorder

	li	$8,-1			# 0xffffffff
$L38:
	lw	$3,0($6)
	subu	$7,$7,1
	sll	$2,$3,1
	or	$2,$2,$5
	sw	$2,0($6)
	srl	$5,$3,31
	.set	noreorder
	.set	nomacro
	bne	$7,$8,$L38
	addu	$6,$6,4
	.set	macro
	.set	reorder

$L37:
	sra	$7,$4,2
	subu	$7,$7,1
	li	$2,-1			# 0xffffffff
	beq	$7,$2,$L40
	li	$8,-1			# 0xffffffff
$L41:
	lw	$2,0($6)
	#nop
	srl	$3,$2,31
	sll	$2,$2,1
	or	$2,$2,$5
	sw	$2,0($6)
	lw	$2,4($6)
	#nop
	srl	$4,$2,31
	sll	$2,$2,1
	or	$2,$2,$3
	sw	$2,4($6)
	lw	$2,8($6)
	#nop
	srl	$3,$2,31
	sll	$2,$2,1
	or	$2,$2,$4
	sw	$2,8($6)
	lw	$2,12($6)
	subu	$7,$7,1
	srl	$5,$2,31
	sll	$2,$2,1
	or	$2,$2,$3
	sw	$2,12($6)
	.set	noreorder
	.set	nomacro
	bne	$7,$8,$L41
	addu	$6,$6,16
	.set	macro
	.set	reorder

$L40:
	.set	noreorder
	.set	nomacro
	j	$31
	andi	$2,$5,0x00ff
	.set	macro
	.set	reorder

	.end	p_rotl

	.loc	1 244
	.ent	p_setp
p_setp:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	j	$31
	.end	p_setp

	.loc	1 268
	.ent	p_smula
p_smula:
	.frame	$sp,8,$31		# vars= 8, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	subu	$sp,$sp,8
	lh	$2,global_precision
	move	$11,$4
	move	$10,$5
	.set	noreorder
	.set	nomacro
	blez	$2,$L43
	move	$13,$2
	.set	macro
	.set	reorder

	lw	$14,0($10)
 #APP
	multu	$6, $14
	mflo	$9
	mfhi	$12
 #NO_APP
	addu	$10,$10,4
	move	$7,$0
	.set	noreorder
	.set	nomacro
	j	$L48
	move	$8,$0
	.set	macro
	.set	reorder

$L47:
	lw	$14,0($10)
 #APP
	multu	$6, $14
	mflo	$4
	mfhi	$5
 #NO_APP
	lw	$2,0($11)
	addu	$10,$10,4
	addu	$8,$8,$12
	addu	$7,$7,$2
	sltu	$3,$7,$2
	addu	$7,$7,$9
	sltu	$2,$7,$9
	addu	$3,$3,$2
	sltu	$2,$8,$12
	addu	$8,$8,$3
	sltu	$3,$8,$3
	sw	$7,0($11)
	addu	$11,$11,4
	move	$7,$8
	addu	$8,$2,$3
	move	$9,$4
	move	$12,$5
$L48:
	subu	$2,$13,1
	move	$13,$2
	sll	$2,$2,16
	bne	$2,$0,$L47
	lw	$2,0($11)
	#nop
	addu	$7,$7,$2
	sltu	$3,$7,$2
	addu	$7,$7,$9
	sltu	$2,$7,$9
	addu	$3,$3,$2
	sw	$7,0($11)
	addu	$11,$11,4
	lw	$2,0($11)
	addu	$8,$8,$12
	addu	$8,$8,$3
	addu	$2,$8,$2
	sw	$2,0($11)
$L43:
	addu	$sp,$sp,8
	j	$31
	.end	p_smula

	.loc	1 334
	.ent	p_setrecip
p_setrecip:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	sw	$4,reciph
	sw	$5,recipl
	sw	$6,mshift
	j	$31
	.end	p_setrecip

	.loc	1 342
	.ent	p_quo_digit
p_quo_digit:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args = 0, extra= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	move	$9,$4
	lw	$2,-8($9)
	lw	$10,reciph
	lw	$5,-4($9)
	nor	$2,$0,$2
 #APP
	multu	$2, $10
	mflo	$6
	mfhi	$7
 #NO_APP
	nor	$5,$0,$5
	lw	$10,recipl
 #APP
	multu	$5, $10
	mflo	$4
	mfhi	$3
 #NO_APP
	addu	$3,$3,1
	lw	$2,reciph
	srl	$4,$4,1
	lw	$10,reciph
	addu	$6,$6,$2
	sltu	$2,$6,$2
	addu	$7,$7,$2
	srl	$6,$6,1
	sll	$2,$7,31
	addu	$6,$6,$2
	srl	$7,$7,1
	sll	$2,$3,31
	addu	$4,$4,$2
	srl	$3,$3,1
	addu	$2,$6,$4
	addu	$3,$7,$3
	sltu	$4,$2,$6
	addu	$4,$4,$3
	addu	$2,$2,1
	sltu	$2,$2,1
	addu	$4,$4,$2
 #APP
	multu	$5, $10
	mflo	$3
	mfhi	$7
 #NO_APP
	srl	$3,$3,1
	addu	$4,$4,1
	lw	$2,0($9)
	sltu	$5,$4,1
	lw	$10,recipl
	nor	$2,$0,$2
 #APP
	multu	$2, $10
	mflo	$6
	mfhi	$8
 #NO_APP
	sll	$2,$7,31
	addu	$3,$3,$2
	srl	$7,$7,1
	srl	$6,$6,1
	sll	$2,$8,31
	addu	$6,$6,$2
	addu	$4,$4,$3
	sltu	$2,$4,$3
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L51
	srl	$8,$8,1
	.set	macro
	.set	reorder

	addu	$5,$5,1
$L51:
	addu	$4,$4,$6
	sltu	$2,$4,$6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L53
	addu	$5,$5,$7
	.set	macro
	.set	reorder

	addu	$5,$5,1
$L53:
	addu	$5,$5,$8
	lw	$2,0($9)
	srl	$4,$4,30
	lw	$10,reciph
	nor	$2,$0,$2
 #APP
	multu	$2, $10
	mflo	$6
	mfhi	$7
 #NO_APP
	sll	$7,$7,1
	srl	$2,$6,31
	addu	$7,$7,$2
	sll	$6,$6,1
	sll	$2,$5,2
	addu	$4,$4,$2
	addu	$4,$4,$6
	sltu	$2,$4,$6
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L55
	srl	$5,$5,30
	.set	macro
	.set	reorder

	addu	$5,$5,1
$L55:
	lw	$3,mshift
	li	$2,0x00000020		# 32
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L57
	addu	$5,$5,$7
	.set	macro
	.set	reorder

	srl	$4,$4,$3
	subu	$2,$2,$3
	sll	$2,$5,$2
	addu	$4,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L58
	srl	$5,$5,$3
	.set	macro
	.set	reorder

$L57:
	move	$4,$5
	move	$5,$0
$L58:
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L59
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

	move	$2,$4
$L59:
	j	$31
	.end	p_quo_digit

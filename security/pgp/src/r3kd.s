/* Multiprecision multiply routine. . .  This assumes the ability to
 * do a 32x32->64 multiply.  I have put in dummy code for the multiplies
 * which should be easily identifiable in the resulting source.
 * This code was designed for the MIPS series processors, however
 * should work pretty well on any machine with hardware multiply.
 * The lack of a carry bit in the R3000 resulted in some of the
 * machinations which take place.  
 *
 * See code in dmul_proto.c for the prototype, and appropriate
 * documentation.
 *
 *  Note that this has hooks for both byte orderings of MIPS processors,
 *  and that if you have the wrong flags being set in your cpp,
 *  you may wind up with no code at all.  (DEC's version will, when
 *  compiling for the other byte ordering, leave BOTH things defined,
 *  which is bad. . . )
 *
 *	- Castor Fu Wed Sep 30 11:48:38 PDT 1992
 */

#if	defined(MIPSEL) && !defined(MIPSEB)
	.verstamp	2 10
	.extern	global_precision 2
	.text	
	.align	2
	.file	2 "ndmulprotoEL.c"
	.globl	p_dmul
	.loc	2 69
 #  69	{
	.ent	p_dmul 2
p_dmul:
	.option	O2
	subu	$sp, 168
	sd	$30, 32($sp)
	sd	$22, 24($sp)
	sd	$20, 16($sp)
	sd	$18, 8($sp)
	sd	$16, 0($sp)
	.mask	0xC0FF0000, -132
	.frame	$sp, 168, $31
	sw	$5, 172($sp)
	sw	$6, 176($sp)
	.loc	2 76
 #  76		preca = global_precision;
	lh	$14, global_precision
	sw	$14, 164($sp)
	.loc	2 77
 #  77		pp=msbptr(multiplicand,preca);
	.loc	2 78
 #  78		while(*post_lowerunit(pp) == 0 && preca > 0) preca--;
	mul	$2, $14, 4
	lw	$15, 172($sp)
	addu	$3, $15, $2
	lw	$11, -4($3)
	addu	$20, $3, -8
	bne	$11, 0, $33
	ble	$14, 0, $33
$32:
	lw	$24, 164($sp)
	addu	$25, $24, -1
	sw	$25, 164($sp)
	lw	$11, 0($20)
	addu	$20, $20, -4
	bne	$11, 0, $33
	bgt	$25, 0, $32
$33:
	.loc	2 80
 #  79	
 #  80		precb = global_precision;
	lh	$15, global_precision
	sw	$15, 160($sp)
	.loc	2 81
 #  81		pp = msbptr(multiplier,precb);
	.loc	2 82
 #  82		while(*post_lowerunit(pp) == 0 && precb > 0) precb--;
	lw	$14, 176($sp)
	addu	$3, $14, $2
	lw	$11, -4($3)
	addu	$20, $3, -8
	bne	$11, 0, $35
	ble	$15, 0, $35
$34:
	lw	$24, 160($sp)
	addu	$25, $24, -1
	sw	$25, 160($sp)
	lw	$11, 0($20)
	addu	$20, $20, -4
	bne	$11, 0, $35
	bgt	$25, 0, $34
$35:
	.loc	2 84
 #  83	
 #  84		prec2 = global_precision<<1;
	lh	$3, global_precision
	sll	$3, $3, 1
	move	$2, $3
	.loc	2 86
 #  85	
 #  86		pp = msbptr(prod,prec2);
	mul	$14, $3, 4
	addu	$20, $4, $14
	addu	$20, $20, -4
	.loc	2 87
 #  87		for (i= preca+precb; i < prec2 ; i++) {
	lw	$15, 164($sp)
	lw	$24, 160($sp)
	addu	$25, $15, $24
	sw	$25, 72($sp)
	move	$21, $25
	bge	$25, $3, $37
$36:
	.loc	2 88
 #  88			*post_lowerunit(pp) = 0;
	sw	$0, 0($20)
	addu	$20, $20, -4
	.loc	2 89
 #  89		}
	.loc	2 89
	addu	$21, $21, 1
	blt	$21, $2, $36
$37:
	.loc	2 90
 #  90		if (preca == 0 || precb == 0) {
	lw	$14, 164($sp)
	beq	$14, 0, $38
	lw	$15, 160($sp)
	bne	$15, 0, $41
$38:
	.loc	2 91
 #  91		  pp = lsbptr(prod,prec2);
	move	$20, $4
	.loc	2 92
 #  92		  for (i=0; i < preca + precb; i++)
	move	$21, $0
	lw	$24, 72($sp)
	ble	$24, 0, $61
	and	$2, $24, 3
	beq	$2, 0, $40
$39:
	sw	$0, 0($20)
	addu	$20, $20, 4
	addu	$21, $21, 1
	bne	$2, $21, $39
	lw	$25, 72($sp)
	beq	$21, $25, $61
$40:
	.loc	2 93
 #  93		    *(post_higherunit(pp)) = 0;
	sw	$0, 0($20)
	addu	$20, $20, 4
	sw	$0, 0($20)
	addu	$20, $20, 4
	sw	$0, 0($20)
	addu	$20, $20, 4
	sw	$0, 0($20)
	addu	$20, $20, 4
	addu	$21, $21, 4
	lw	$14, 72($sp)
	bne	$21, $14, $40
	.loc	2 94
 #  94		  return;
	b	$61
$41:
	.loc	2 97
 #  95		}
 #  96		/* Canonicalize length(a) >= length(b) */
 #  97		if (preca < precb) {
	lw	$15, 164($sp)
	lw	$24, 160($sp)
	bge	$15, $24, $42
	.loc	2 98
 #  98		  pp = multiplicand;
	lw	$20, 172($sp)
	.loc	2 99
 #  99		  multiplicand = multiplier;
	lw	$25, 176($sp)
	sw	$25, 172($sp)
	.loc	2 100
 # 100		  multiplier = pp;
	sw	$20, 176($sp)
	.loc	2 101
 # 101		  i = preca;
	move	$21, $15
	.loc	2 102
 # 102		  preca = precb;
	sw	$24, 164($sp)
	.loc	2 103
 # 103		  precb = i;
	sw	$21, 160($sp)
	.loc	2 104
 # 104		}
	lw	$14, 164($sp)
	lw	$25, 160($sp)
	addu	$15, $14, $25
	sw	$15, 72($sp)
$42:
	.loc	2 107
 # 105		make_lsbptr(multiplicand, global_precision);
 # 106		make_lsbptr(multiplier, global_precision);
 # 107		pp = lsbptr(prod,prec2);
	move	$20, $4
	.loc	2 108
 # 108		ah = 0;
	move	$12, $0
	.loc	2 109
 # 109		carryl = 0;
	move	$19, $0
	.loc	2 110
 # 110		carryh =0;
	move	$18, $0
	.loc	2 111
 # 111		al = 0;
	.loc	2 114
 # 112	
 # 113	
 # 114		amin = 0;
	.loc	2 115
 # 115		for (i=0; i < precb; i++) {
	move	$21, $0
	lw	$24, 160($sp)
	ble	$24, 0, $48
	lw	$23, 172($sp)
	addu	$30, $23, -4
	lw	$14, 176($sp)
	addu	$25, $14, 4
	sw	$25, 56($sp)
$43:
	.loc	2 116
 # 116			amax = i;
	.loc	2 117
 # 117			ma = word_index(multiplicand,i -1);
	move	$13, $30
	.loc	2 118
 # 118			mb = word_index(multiplier,1);
	lw	$16, 56($sp)
	.loc	2 119
 # 119				lmul(word_v(ma,1),word_v(mb,-1),ml, mh);
	lw	$2, 0($23)
	lw	$15, 176($sp)
	lw	$3, 0($15)
	multu	$2, $3
	mflo	$9
	mfhi	$10
	.loc	2 120
 # 120				al = ah+ carryl;
	addu	$4, $12, $19
	move	$8, $4
	.loc	2 121
 # 121				carryl = carryh + (al < ah);
	sltu	$24, $4, $12
	addu	$19, $18, $24
	.loc	2 122
 # 122				ah = 0;
	move	$12, $0
	.loc	2 123
 # 123				carryh = 0;
	move	$18, $0
	.loc	2 124
 # 124			j = amax - amin;
	move	$5, $21
	move	$31, $5
	.loc	2 125
 # 125			k = j & 3;
	.loc	2 126
 # 126			while (k--) {
	and	$6, $5, 3
	addu	$17, $6, -1
	beq	$6, $0, $45
$44:
	.loc	2 127
 # 127				lmul( *ma, *mb, nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 128
 # 128				al += ml;
	addu	$8, $8, $9
	.loc	2 129
 # 129				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 130
 # 130				ah += mh;
	addu	$12, $12, $10
	.loc	2 131
 # 131				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 132
 # 132				post_lowerunit(ma);
	addu	$13, $13, -4
	.loc	2 133
 # 133				post_higherunit(mb);
	addu	$16, $16, 4
	.loc	2 134
 # 134				ml = nml;
	mflo	$9
	.loc	2 135
 # 135				mh = nmh;
	mfhi	$10
	.loc	2 136
 # 136			}
	.loc	2 136
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $44
$45:
	.loc	2 137
 # 137			k = j >> 2;
	.loc	2 138
 # 138			while (k--) {
	sra	$2, $31, 2
	addu	$17, $2, -1
	beq	$2, $0, $47
$46:
	.loc	2 139
 # 139				lmul( word_v(ma,0), word_v(mb,0), nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 140
 # 140				al += ml;
	addu	$8, $8, $9
	.loc	2 141
 # 141				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 142
 # 142				ah += mh;
	addu	$12, $12, $10
	.loc	2 143
 # 143				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 144
 # 144				ml = nml;
	mflo	$9
	.loc	2 145
 # 145				mh = nmh;
	mfhi	$10
	.loc	2 147
 # 146	
 # 147				lmul( word_v(ma,-1), word_v(mb,1), nml, nmh);
	lw	$4, -4($13)
	lw	$5, 4($16)
	multu	$4, $5
	.loc	2 148
 # 148				al += ml;
	addu	$8, $8, $9
	.loc	2 149
 # 149				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 150
 # 150				ah += mh;
	addu	$12, $12, $10
	.loc	2 151
 # 151				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 152
 # 152				ml = nml;
	mflo	$9
	.loc	2 153
 # 153				mh = nmh;
	mfhi	$10
	.loc	2 155
 # 154	
 # 155				lmul( word_v(ma,-2), word_v(mb,2), nml, nmh);
	lw	$2, -8($13)
	lw	$3, 8($16)
	multu	$2, $3
	.loc	2 156
 # 156				al += ml;
	addu	$8, $8, $9
	.loc	2 157
 # 157				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 158
 # 158				ah += mh;
	addu	$12, $12, $10
	.loc	2 159
 # 159				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 160
 # 160				ml = nml;
	mflo	$9
	.loc	2 161
 # 161				mh = nmh;
	mfhi	$10
	.loc	2 163
 # 162	
 # 163				lmul( word_v(ma,-3), word_v(mb,3), nml, nmh);
	lw	$4, -12($13)
	lw	$5, 12($16)
	multu	$4, $5
	.loc	2 164
 # 164				al += ml;
	addu	$8, $8, $9
	.loc	2 165
 # 165				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 166
 # 166				ah += mh;
	addu	$12, $12, $10
	.loc	2 167
 # 167				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 168
 # 168				ml = nml;
	mflo	$9
	.loc	2 169
 # 169				mh = nmh;
	mfhi	$10
	.loc	2 171
 # 170	
 # 171				nlowerunit(ma,4); nhigherunit(mb,4);
	addu	$13, $13, -16
	addu	$16, $16, 16
	.loc	2 173
 # 172	
 # 173			}
	.loc	2 173
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $46
$47:
	.loc	2 175
 # 174	
 # 175			al += ml;
	addu	$8, $8, $9
	.loc	2 176
 # 176			carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 177
 # 177			*pp = al;
	sw	$8, 0($20)
	.loc	2 178
 # 178			ah += mh;
	addu	$12, $12, $10
	.loc	2 179
 # 179			carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 180
 # 180			post_higherunit(pp);
	addu	$20, $20, 4
	.loc	2 181
 # 181		}
	.loc	2 181
	addu	$21, $21, 1
	addu	$23, $23, 4
	addu	$30, $30, 4
	lw	$14, 160($sp)
	bne	$21, $14, $43
$48:
	.loc	2 182
 # 182		amax = precb-1;
	.loc	2 183
 # 183		for (i=precb; i < preca; i++) {
	lw	$25, 160($sp)
	move	$21, $25
	lw	$15, 164($sp)
	bge	$25, $15, $54
	mul	$22, $21, 4
	lw	$24, 172($sp)
	addu	$23, $24, $22
	addu	$30, $23, -4
	lw	$14, 176($sp)
	addu	$24, $14, 4
	sw	$24, 56($sp)
	mul	$14, $15, 4
	sw	$14, 52($sp)
	addu	$2, $25, -1
	move	$31, $2
	and	$24, $2, 3
	sw	$24, 44($sp)
	addu	$15, $24, -1
	sw	$15, 40($sp)
$49:
	.loc	2 184
 # 184			ma = word_index( multiplicand,i-1);
	move	$13, $30
	.loc	2 185
 # 185			mb = word_index(multiplier,1);
	lw	$16, 56($sp)
	.loc	2 186
 # 186				lmul(word_v(ma,1),word_v(mb,-1),ml, mh);
	lw	$2, 0($23)
	lw	$14, 176($sp)
	lw	$3, 0($14)
	multu	$2, $3
	mflo	$9
	mfhi	$10
	.loc	2 187
 # 187				al = ah+ carryl;
	addu	$4, $12, $19
	move	$8, $4
	.loc	2 188
 # 188				carryl = carryh + (al < ah);
	sltu	$25, $4, $12
	addu	$19, $18, $25
	.loc	2 189
 # 189				ah = 0;
	move	$12, $0
	.loc	2 190
 # 190				carryh = 0;
	move	$18, $0
	.loc	2 191
 # 191			j = amax - amin;
	.loc	2 192
 # 192			k = j & 3;
	.loc	2 193
 # 193			while (k--) {
	lw	$17, 40($sp)
	lw	$24, 44($sp)
	beq	$24, $0, $51
$50:
	.loc	2 194
 # 194				lmul( *ma, *mb, nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 195
 # 195				al += ml;
	addu	$8, $8, $9
	.loc	2 196
 # 196				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 197
 # 197				ah += mh;
	addu	$12, $12, $10
	.loc	2 198
 # 198				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 199
 # 199				post_lowerunit(ma);
	addu	$13, $13, -4
	.loc	2 200
 # 200				post_higherunit(mb);
	addu	$16, $16, 4
	.loc	2 201
 # 201				ml = nml;
	mflo	$9
	.loc	2 202
 # 202				mh = nmh;
	mfhi	$10
	.loc	2 203
 # 203			}
	.loc	2 203
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $50
$51:
	.loc	2 204
 # 204			k = j >> 2;
	.loc	2 205
 # 205			while (k--) {
	sra	$2, $31, 2
	addu	$17, $2, -1
	beq	$2, $0, $53
$52:
	.loc	2 206
 # 206				lmul( word_v(ma,0), word_v(mb,0), nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 207
 # 207				al += ml;
	addu	$8, $8, $9
	.loc	2 208
 # 208				carryl += (al < ml);
	sltu	$25, $8, $9
	addu	$19, $19, $25
	.loc	2 209
 # 209				ah += mh;
	addu	$12, $12, $10
	.loc	2 210
 # 210				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 211
 # 211				ml = nml;
	mflo	$9
	.loc	2 212
 # 212				mh = nmh;
	mfhi	$10
	.loc	2 214
 # 213	
 # 214				lmul( word_v(ma,-1), word_v(mb,1), nml, nmh);
	lw	$4, -4($13)
	lw	$5, 4($16)
	multu	$4, $5
	.loc	2 215
 # 215				al += ml;
	addu	$8, $8, $9
	.loc	2 216
 # 216				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 217
 # 217				ah += mh;
	addu	$12, $12, $10
	.loc	2 218
 # 218				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 219
 # 219				ml = nml;
	mflo	$9
	.loc	2 220
 # 220				mh = nmh;
	mfhi	$10
	.loc	2 222
 # 221	
 # 222				lmul( word_v(ma,-2), word_v(mb,2), nml, nmh);
	lw	$2, -8($13)
	lw	$3, 8($16)
	multu	$2, $3
	.loc	2 223
 # 223				al += ml;
	addu	$8, $8, $9
	.loc	2 224
 # 224				carryl += (al < ml);
	sltu	$25, $8, $9
	addu	$19, $19, $25
	.loc	2 225
 # 225				ah += mh;
	addu	$12, $12, $10
	.loc	2 226
 # 226				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 227
 # 227				ml = nml;
	mflo	$9
	.loc	2 228
 # 228				mh = nmh;
	mfhi	$10
	.loc	2 230
 # 229	
 # 230				lmul( word_v(ma,-3), word_v(mb,3), nml, nmh);
	lw	$4, -12($13)
	lw	$5, 12($16)
	multu	$4, $5
	.loc	2 231
 # 231				al += ml;
	addu	$8, $8, $9
	.loc	2 232
 # 232				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 233
 # 233				ah += mh;
	addu	$12, $12, $10
	.loc	2 234
 # 234				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 235
 # 235				ml = nml;
	mflo	$9
	.loc	2 236
 # 236				mh = nmh;
	mfhi	$10
	.loc	2 238
 # 237	
 # 238				nlowerunit(ma,4);
	addu	$13, $13, -16
	.loc	2 239
 # 239				nhigherunit(mb,4);
	addu	$16, $16, 16
	.loc	2 240
 # 240			}
	.loc	2 240
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $52
$53:
	.loc	2 242
 # 241	
 # 242			al += ml;
	addu	$8, $8, $9
	.loc	2 243
 # 243			carryl += (al < ml);
	sltu	$25, $8, $9
	addu	$19, $19, $25
	.loc	2 244
 # 244			*pp = al;
	sw	$8, 0($20)
	.loc	2 245
 # 245			ah += mh;
	addu	$12, $12, $10
	.loc	2 246
 # 246			carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 247
 # 247			post_higherunit(pp);
	addu	$20, $20, 4
	.loc	2 248
 # 248		}
	.loc	2 248
	addu	$22, $22, 4
	addu	$23, $23, 4
	addu	$30, $30, 4
	lw	$15, 52($sp)
	blt	$22, $15, $49
$54:
	.loc	2 249
 # 249		amax = precb-1;
	.loc	2 250
 # 250		for (i=preca; i < preca + precb -1; i++) {
	lw	$14, 164($sp)
	move	$21, $14
	lw	$25, 72($sp)
	addu	$24, $25, -1
	sw	$24, 68($sp)
	bge	$14, $24, $60
	lw	$15, 160($sp)
	addu	$25, $15, -1
	sw	$25, 72($sp)
	lw	$24, 172($sp)
	mul	$15, $14, 4
	addu	$30, $24, $15
	addu	$25, $30, -8
	sw	$25, 60($sp)
	lw	$24, 176($sp)
	subu	$15, $21, $14
	mul	$25, $15, 4
	addu	$22, $24, $25
	addu	$23, $22, 8
$55:
	.loc	2 251
 # 251			amin = i-preca + 1;
	.loc	2 252
 # 252			ma = word_index(multiplicand,preca-2);
	lw	$13, 60($sp)
	.loc	2 253
 # 253			mb = word_index(multiplier, - preca + 2 +i) ;
	move	$16, $23
	.loc	2 254
 # 254				lmul(word_v(ma,1),word_v(mb,-1),ml, mh);
	lw	$2, -4($30)
	lw	$3, 4($22)
	multu	$2, $3
	mflo	$9
	mfhi	$10
	.loc	2 255
 # 255				al = ah+ carryl;
	addu	$4, $12, $19
	move	$8, $4
	.loc	2 256
 # 256				carryl = carryh + (al < ah);
	sltu	$14, $4, $12
	addu	$19, $18, $14
	.loc	2 257
 # 257				ah = 0;
	move	$12, $0
	.loc	2 258
 # 258				carryh = 0;
	move	$18, $0
	.loc	2 259
 # 259			j = amax - amin;
	lw	$15, 72($sp)
	lw	$24, 164($sp)
	subu	$25, $21, $24
	subu	$5, $15, $25
	addu	$5, $5, -1
	move	$31, $5
	.loc	2 260
 # 260			k = j & 3;
	.loc	2 261
 # 261			while (k--) {
	and	$6, $5, 3
	addu	$17, $6, -1
	beq	$6, $0, $57
$56:
	.loc	2 262
 # 262				lmul( *ma, *mb, nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 263
 # 263				al += ml;
	addu	$8, $8, $9
	.loc	2 264
 # 264				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 265
 # 265				ah += mh;
	addu	$12, $12, $10
	.loc	2 266
 # 266				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 267
 # 267				post_lowerunit(ma);
	addu	$13, $13, -4
	.loc	2 268
 # 268				post_higherunit(mb);
	addu	$16, $16, 4
	.loc	2 269
 # 269				ml = nml;
	mflo	$9
	.loc	2 270
 # 270				mh = nmh;
	mfhi	$10
	.loc	2 271
 # 271			}
	.loc	2 271
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $56
$57:
	.loc	2 272
 # 272			k = j >> 2;
	.loc	2 273
 # 273			while (k--) {
	sra	$2, $31, 2
	addu	$17, $2, -1
	beq	$2, $0, $59
$58:
	.loc	2 274
 # 274				lmul( word_v(ma,0), word_v(mb,0), nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 275
 # 275				al += ml;
	addu	$8, $8, $9
	.loc	2 276
 # 276				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 277
 # 277				ah += mh;
	addu	$12, $12, $10
	.loc	2 278
 # 278				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 279
 # 279				ml = nml;
	mflo	$9
	.loc	2 280
 # 280				mh = nmh;
	mfhi	$10
	.loc	2 282
 # 281	
 # 282				lmul( word_v(ma,-1), word_v(mb,1), nml, nmh);
	lw	$4, -4($13)
	lw	$5, 4($16)
	multu	$4, $5
	.loc	2 283
 # 283				al += ml;
	addu	$8, $8, $9
	.loc	2 284
 # 284				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 285
 # 285				ah += mh;
	addu	$12, $12, $10
	.loc	2 286
 # 286				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 287
 # 287				ml = nml;
	mflo	$9
	.loc	2 288
 # 288				mh = nmh;
	mfhi	$10
	.loc	2 290
 # 289	
 # 290				lmul( word_v(ma,-2), word_v(mb,2), nml, nmh);
	lw	$2, -8($13)
	lw	$3, 8($16)
	multu	$2, $3
	.loc	2 291
 # 291				al += ml;
	addu	$8, $8, $9
	.loc	2 292
 # 292				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 293
 # 293				ah += mh;
	addu	$12, $12, $10
	.loc	2 294
 # 294				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 295
 # 295				ml = nml;
	mflo	$9
	.loc	2 296
 # 296				mh = nmh;
	mfhi	$10
	.loc	2 298
 # 297	
 # 298				lmul( word_v(ma,-3), word_v(mb,3), nml, nmh);
	lw	$4, -12($13)
	lw	$5, 12($16)
	multu	$4, $5
	.loc	2 299
 # 299				al += ml;
	addu	$8, $8, $9
	.loc	2 300
 # 300				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 301
 # 301				ah += mh;
	addu	$12, $12, $10
	.loc	2 302
 # 302				carryh += (ah < mh);
	sltu	$24, $12, $10
	addu	$18, $18, $24
	.loc	2 303
 # 303				ml = nml;
	mflo	$9
	.loc	2 304
 # 304				mh = nmh;
	mfhi	$10
	.loc	2 306
 # 305	
 # 306				nlowerunit(ma,4);
	addu	$13, $13, -16
	.loc	2 307
 # 307				nhigherunit(mb,4);
	addu	$16, $16, 16
	.loc	2 308
 # 308			}
	.loc	2 308
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $58
$59:
	.loc	2 310
 # 309	
 # 310			al += ml;
	addu	$8, $8, $9
	.loc	2 311
 # 311			carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 312
 # 312			*pp = al;
	sw	$8, 0($20)
	.loc	2 313
 # 313			ah += mh;
	addu	$12, $12, $10
	.loc	2 314
 # 314			carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 315
 # 315			post_higherunit(pp);
	addu	$20, $20, 4
	.loc	2 316
 # 316		}
	.loc	2 316
	addu	$21, $21, 1
	addu	$22, $22, 4
	addu	$23, $23, 4
	lw	$14, 68($sp)
	blt	$21, $14, $55
$60:
	.loc	2 317
 # 317		al = ah + carryl;
	.loc	2 318
 # 318		carryh += (al < ah);
	.loc	2 319
 # 319		*pp  = al;
	addu	$24, $12, $19
	sw	$24, 0($20)
	.loc	2 320
 # 320	}
$61:
	ld	$16, 0($sp)
	ld	$18, 8($sp)
	ld	$20, 16($sp)
	ld	$22, 24($sp)
	ld	$30, 32($sp)
	addu	$sp, 168
	j	$31
	.end	p_dmul
#endif

#if	defined(MIPSEB) && !defined(MIPSEL)
	.verstamp	2 10
	.extern	global_precision 2
	.text	
	.align	2
	.file	2 "ndmulprotoEB.c"
	.globl	p_dmul
	.loc	2 69
 #  69	{
	.ent	p_dmul 2
p_dmul:
	.option	O2
	subu	$sp, 168
	sd	$30, 32($sp)
	sd	$22, 24($sp)
	sd	$20, 16($sp)
	sd	$18, 8($sp)
	sd	$16, 0($sp)
	.mask	0xC0FF0000, -132
	.frame	$sp, 168, $31
	sw	$5, 172($sp)
	sw	$6, 176($sp)
	.loc	2 76
 #  76		preca = global_precision;
	lh	$14, global_precision
	sw	$14, 164($sp)
	.loc	2 77
 #  77		pp=msbptr(multiplicand,preca);
	.loc	2 78
 #  78		while(*post_lowerunit(pp) == 0 && preca > 0) preca--;
	lw	$15, 172($sp)
	lw	$11, 0($15)
	addu	$20, $15, 4
	bne	$11, 0, $33
	ble	$14, 0, $33
$32:
	lw	$24, 164($sp)
	addu	$25, $24, -1
	sw	$25, 164($sp)
	lw	$11, 0($20)
	addu	$20, $20, 4
	bne	$11, 0, $33
	bgt	$25, 0, $32
$33:
	.loc	2 80
 #  79	
 #  80		precb = global_precision;
	lh	$15, global_precision
	sw	$15, 160($sp)
	.loc	2 81
 #  81		pp = msbptr(multiplier,precb);
	.loc	2 82
 #  82		while(*post_lowerunit(pp) == 0 && precb > 0) precb--;
	lw	$14, 176($sp)
	lw	$11, 0($14)
	addu	$20, $14, 4
	bne	$11, 0, $35
	ble	$15, 0, $35
$34:
	lw	$24, 160($sp)
	addu	$25, $24, -1
	sw	$25, 160($sp)
	lw	$11, 0($20)
	addu	$20, $20, 4
	bne	$11, 0, $35
	bgt	$25, 0, $34
$35:
	.loc	2 84
 #  83	
 #  84		prec2 = global_precision<<1;
	lh	$3, global_precision
	sll	$3, $3, 1
	move	$2, $3
	.loc	2 86
 #  85	
 #  86		pp = msbptr(prod,prec2);
	move	$20, $4
	.loc	2 87
 #  87		for (i= preca+precb; i < prec2 ; i++) {
	lw	$14, 164($sp)
	lw	$15, 160($sp)
	addu	$24, $14, $15
	sw	$24, 72($sp)
	move	$21, $24
	bge	$24, $3, $37
$36:
	.loc	2 88
 #  88			*post_lowerunit(pp) = 0;
	sw	$0, 0($20)
	addu	$20, $20, 4
	.loc	2 89
 #  89		}
	.loc	2 89
	addu	$21, $21, 1
	blt	$21, $2, $36
$37:
	.loc	2 90
 #  90		if (preca == 0 || precb == 0) {
	lw	$25, 164($sp)
	beq	$25, 0, $38
	lw	$14, 160($sp)
	bne	$14, 0, $41
$38:
	.loc	2 91
 #  91		  pp = lsbptr(prod,prec2);
	mul	$15, $2, 4
	addu	$20, $4, $15
	addu	$20, $20, -4
	.loc	2 92
 #  92		  for (i=0; i < preca + precb; i++)
	move	$21, $0
	lw	$24, 72($sp)
	ble	$24, 0, $61
	and	$2, $24, 3
	beq	$2, 0, $40
$39:
	sw	$0, 0($20)
	addu	$20, $20, -4
	addu	$21, $21, 1
	bne	$2, $21, $39
	lw	$25, 72($sp)
	beq	$21, $25, $61
$40:
	.loc	2 93
 #  93		    *(post_higherunit(pp)) = 0;
	sw	$0, 0($20)
	addu	$20, $20, -4
	sw	$0, 0($20)
	addu	$20, $20, -4
	sw	$0, 0($20)
	addu	$20, $20, -4
	sw	$0, 0($20)
	addu	$20, $20, -4
	addu	$21, $21, 4
	lw	$14, 72($sp)
	bne	$21, $14, $40
	.loc	2 94
 #  94		  return;
	b	$61
$41:
	.loc	2 97
 #  95		}
 #  96		/* Canonicalize length(a) >= length(b) */
 #  97		if (preca < precb) {
	lw	$15, 164($sp)
	lw	$24, 160($sp)
	bge	$15, $24, $42
	.loc	2 98
 #  98		  pp = multiplicand;
	lw	$20, 172($sp)
	.loc	2 99
 #  99		  multiplicand = multiplier;
	lw	$25, 176($sp)
	sw	$25, 172($sp)
	.loc	2 100
 # 100		  multiplier = pp;
	sw	$20, 176($sp)
	.loc	2 101
 # 101		  i = preca;
	move	$21, $15
	.loc	2 102
 # 102		  preca = precb;
	sw	$24, 164($sp)
	.loc	2 103
 # 103		  precb = i;
	sw	$21, 160($sp)
	.loc	2 104
 # 104		}
	lw	$14, 164($sp)
	lw	$25, 160($sp)
	addu	$15, $14, $25
	sw	$15, 72($sp)
$42:
	.loc	2 105
 # 105		make_lsbptr(multiplicand, global_precision);
	lh	$3, global_precision
	mul	$3, $3, 4
	lw	$24, 172($sp)
	addu	$14, $24, $3
	addu	$25, $14, -4
	sw	$25, 172($sp)
	.loc	2 106
 # 106		make_lsbptr(multiplier, global_precision);
	lw	$15, 176($sp)
	addu	$24, $15, $3
	addu	$14, $24, -4
	sw	$14, 176($sp)
	.loc	2 107
 # 107		pp = lsbptr(prod,prec2);
	mul	$15, $2, 4
	addu	$20, $4, $15
	addu	$20, $20, -4
	.loc	2 108
 # 108		ah = 0;
	move	$12, $0
	.loc	2 109
 # 109		carryl = 0;
	move	$19, $0
	.loc	2 110
 # 110		carryh =0;
	move	$18, $0
	.loc	2 111
 # 111		al = 0;
	.loc	2 114
 # 112	
 # 113	
 # 114		amin = 0;
	.loc	2 115
 # 115		for (i=0; i < precb; i++) {
	move	$21, $0
	lw	$24, 160($sp)
	ble	$24, 0, $48
	move	$23, $25
	addu	$30, $23, 4
	addu	$15, $14, -4
	sw	$15, 56($sp)
$43:
	.loc	2 116
 # 116			amax = i;
	.loc	2 117
 # 117			ma = word_index(multiplicand,i -1);
	move	$13, $30
	.loc	2 118
 # 118			mb = word_index(multiplier,1);
	lw	$16, 56($sp)
	.loc	2 119
 # 119				lmul(word_v(ma,1),word_v(mb,-1),ml, mh);
	lw	$3, 0($23)
	lw	$24, 176($sp)
	lw	$2, 0($24)
	multu	$3,$2
	mflo	$9
	mfhi	$10
	.loc	2 120
 # 120				al = ah+ carryl;
	addu	$4, $12, $19
	move	$8, $4
	.loc	2 121
 # 121				carryl = carryh + (al < ah);
	sltu	$25, $4, $12
	addu	$19, $18, $25
	.loc	2 122
 # 122				ah = 0;
	move	$12, $0
	.loc	2 123
 # 123				carryh = 0;
	move	$18, $0
	.loc	2 124
 # 124			j = amax - amin;
	move	$5, $21
	move	$31, $5
	.loc	2 125
 # 125			k = j & 3;
	.loc	2 126
 # 126			while (k--) {
	and	$6, $5, 3
	addu	$17, $6, -1
	beq	$6, $0, $45
$44:
	.loc	2 127
 # 127				lmul( *ma, *mb, nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 128
 # 128				al += ml;
	addu	$8, $8, $9
	.loc	2 129
 # 129				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 130
 # 130				ah += mh;
	addu	$12, $12, $10
	.loc	2 131
 # 131				carryh += (ah < mh);
	sltu	$15, $12, $10
	addu	$18, $18, $15
	.loc	2 132
 # 132				post_lowerunit(ma);
	addu	$13, $13, 4
	.loc	2 133
 # 133				post_higherunit(mb);
	addu	$16, $16, -4
	.loc	2 134
 # 134				ml = nml;
	mflo	$9
	.loc	2 135
 # 135				mh = nmh;
	mfhi	$10
	.loc	2 136
 # 136			}
	.loc	2 136
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $44
$45:
	.loc	2 137
 # 137			k = j >> 2;
	.loc	2 138
 # 138			while (k--) {
	sra	$2, $31, 2
	addu	$17, $2, -1
	beq	$2, $0, $47
$46:
	.loc	2 139
 # 139				lmul( word_v(ma,0), word_v(mb,0), nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 140
 # 140				al += ml;
	addu	$8, $8, $9
	.loc	2 141
 # 141				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 142
 # 142				ah += mh;
	addu	$12, $12, $10
	.loc	2 143
 # 143				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 144
 # 144				ml = nml;
	mflo	$9
	.loc	2 145
 # 145				mh = nmh;
	mfhi	$10
	.loc	2 147
 # 146	
 # 147				lmul( word_v(ma,-1), word_v(mb,1), nml, nmh);
	lw	$4, 4($13)
	lw	$5, -4($16)
	multu	$4, $5
	.loc	2 148
 # 148				al += ml;
	addu	$8, $8, $9
	.loc	2 149
 # 149				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 150
 # 150				ah += mh;
	addu	$12, $12, $10
	.loc	2 151
 # 151				carryh += (ah < mh);
	sltu	$15, $12, $10
	addu	$18, $18, $15
	.loc	2 152
 # 152				ml = nml;
	mflo	$9
	.loc	2 153
 # 153				mh = nmh;
	mfhi	$10
	.loc	2 155
 # 154	
 # 155				lmul( word_v(ma,-2), word_v(mb,2), nml, nmh);
	lw	$2, 8($13)
	lw	$3, -8($16)
	multu	$2, $3
	.loc	2 156
 # 156				al += ml;
	addu	$8, $8, $9
	.loc	2 157
 # 157				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 158
 # 158				ah += mh;
	addu	$12, $12, $10
	.loc	2 159
 # 159				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 160
 # 160				ml = nml;
	mflo	$9
	.loc	2 161
 # 161				mh = nmh;
	mfhi	$10
	.loc	2 163
 # 162	
 # 163				lmul( word_v(ma,-3), word_v(mb,3), nml, nmh);
	lw	$4, 12($13)
	lw	$5, -12($16)
	multu	$4, $5
	.loc	2 164
 # 164				al += ml;
	addu	$8, $8, $9
	.loc	2 165
 # 165				carryl += (al < ml);
	sltu	$14, $8, $9
	addu	$19, $19, $14
	.loc	2 166
 # 166				ah += mh;
	addu	$12, $12, $10
	.loc	2 167
 # 167				carryh += (ah < mh);
	sltu	$15, $12, $10
	addu	$18, $18, $15
	.loc	2 168
 # 168				ml = nml;
	mflo	$9
	.loc	2 169
 # 169				mh = nmh;
	mfhi	$10
	.loc	2 171
 # 170	
 # 171				nlowerunit(ma,4); nhigherunit(mb,4);
	addu	$13, $13, 16
	addu	$16, $16, -16
	.loc	2 173
 # 172	
 # 173			}
	.loc	2 173
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $46
$47:
	.loc	2 175
 # 174	
 # 175			al += ml;
	addu	$8, $8, $9
	.loc	2 176
 # 176			carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 177
 # 177			*pp = al;
	sw	$8, 0($20)
	.loc	2 178
 # 178			ah += mh;
	addu	$12, $12, $10
	.loc	2 179
 # 179			carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 180
 # 180			post_higherunit(pp);
	addu	$20, $20, -4
	.loc	2 181
 # 181		}
	.loc	2 181
	addu	$21, $21, 1
	addu	$23, $23, -4
	addu	$30, $30, -4
	lw	$14, 160($sp)
	bne	$21, $14, $43
$48:
	.loc	2 182
 # 182		amax = precb-1;
	.loc	2 183
 # 183		for (i=precb; i < preca; i++) {
	lw	$15, 160($sp)
	move	$21, $15
	lw	$24, 164($sp)
	bge	$15, $24, $54
	negu	$22, $21
	mul	$22, $22, 4
	lw	$25, 172($sp)
	addu	$23, $25, $22
	addu	$30, $23, 4
	lw	$14, 176($sp)
	addu	$25, $14, -4
	sw	$25, 56($sp)
	mul	$14, $24, -4
	sw	$14, 52($sp)
	addu	$2, $15, -1
	move	$31, $2
	and	$25, $2, 3
	sw	$25, 44($sp)
	addu	$24, $25, -1
	sw	$24, 40($sp)
$49:
	.loc	2 184
 # 184			ma = word_index( multiplicand,i-1);
	move	$13, $30
	.loc	2 185
 # 185			mb = word_index(multiplier,1);
	lw	$16, 56($sp)
	.loc	2 186
 # 186				lmul(word_v(ma,1),word_v(mb,-1),ml, mh);
	lw	$3, 0($23)
	lw	$14, 176($sp)
	lw	$2, 0($14)
	multu	$3,$2
	mflo	$9
	mfhi	$10
	.loc	2 187
 # 187				al = ah+ carryl;
	addu	$4, $12, $19
	move	$8, $4
	.loc	2 188
 # 188				carryl = carryh + (al < ah);
	sltu	$15, $4, $12
	addu	$19, $18, $15
	.loc	2 189
 # 189				ah = 0;
	move	$12, $0
	.loc	2 190
 # 190				carryh = 0;
	move	$18, $0
	.loc	2 191
 # 191			j = amax - amin;
	.loc	2 192
 # 192			k = j & 3;
	.loc	2 193
 # 193			while (k--) {
	lw	$17, 40($sp)
	lw	$25, 44($sp)
	beq	$25, $0, $51
$50:
	.loc	2 194
 # 194				lmul( *ma, *mb, nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 195
 # 195				al += ml;
	addu	$8, $8, $9
	.loc	2 196
 # 196				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 197
 # 197				ah += mh;
	addu	$12, $12, $10
	.loc	2 198
 # 198				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 199
 # 199				post_lowerunit(ma);
	addu	$13, $13, 4
	.loc	2 200
 # 200				post_higherunit(mb);
	addu	$16, $16, -4
	.loc	2 201
 # 201				ml = nml;
	mflo	$9
	.loc	2 202
 # 202				mh = nmh;
	mfhi	$10
	.loc	2 203
 # 203			}
	.loc	2 203
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $50
$51:
	.loc	2 204
 # 204			k = j >> 2;
	.loc	2 205
 # 205			while (k--) {
	sra	$2, $31, 2
	addu	$17, $2, -1
	beq	$2, $0, $53
$52:
	.loc	2 206
 # 206				lmul( word_v(ma,0), word_v(mb,0), nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 207
 # 207				al += ml;
	addu	$8, $8, $9
	.loc	2 208
 # 208				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 209
 # 209				ah += mh;
	addu	$12, $12, $10
	.loc	2 210
 # 210				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 211
 # 211				ml = nml;
	mflo	$9
	.loc	2 212
 # 212				mh = nmh;
	mfhi	$10
	.loc	2 214
 # 213	
 # 214				lmul( word_v(ma,-1), word_v(mb,1), nml, nmh);
	lw	$4, 4($13)
	lw	$5, -4($16)
	multu	$4, $5
	.loc	2 215
 # 215				al += ml;
	addu	$8, $8, $9
	.loc	2 216
 # 216				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 217
 # 217				ah += mh;
	addu	$12, $12, $10
	.loc	2 218
 # 218				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 219
 # 219				ml = nml;
	mflo	$9
	.loc	2 220
 # 220				mh = nmh;
	mfhi	$10
	.loc	2 222
 # 221	
 # 222				lmul( word_v(ma,-2), word_v(mb,2), nml, nmh);
	lw	$2, 8($13)
	lw	$3, -8($16)
	multu	$2, $3
	.loc	2 223
 # 223				al += ml;
	addu	$8, $8, $9
	.loc	2 224
 # 224				carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 225
 # 225				ah += mh;
	addu	$12, $12, $10
	.loc	2 226
 # 226				carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 227
 # 227				ml = nml;
	mflo	$9
	.loc	2 228
 # 228				mh = nmh;
	mfhi	$10
	.loc	2 230
 # 229	
 # 230				lmul( word_v(ma,-3), word_v(mb,3), nml, nmh);
	lw	$4, 12($13)
	lw	$5, -12($16)
	multu	$4, $5
	.loc	2 231
 # 231				al += ml;
	addu	$8, $8, $9
	.loc	2 232
 # 232				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 233
 # 233				ah += mh;
	addu	$12, $12, $10
	.loc	2 234
 # 234				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 235
 # 235				ml = nml;
	mflo	$9
	.loc	2 236
 # 236				mh = nmh;
	mfhi	$10
	.loc	2 238
 # 237	
 # 238				nlowerunit(ma,4);
	addu	$13, $13, 16
	.loc	2 239
 # 239				nhigherunit(mb,4);
	addu	$16, $16, -16
	.loc	2 240
 # 240			}
	.loc	2 240
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $52
$53:
	.loc	2 242
 # 241	
 # 242			al += ml;
	addu	$8, $8, $9
	.loc	2 243
 # 243			carryl += (al < ml);
	sltu	$15, $8, $9
	addu	$19, $19, $15
	.loc	2 244
 # 244			*pp = al;
	sw	$8, 0($20)
	.loc	2 245
 # 245			ah += mh;
	addu	$12, $12, $10
	.loc	2 246
 # 246			carryh += (ah < mh);
	sltu	$25, $12, $10
	addu	$18, $18, $25
	.loc	2 247
 # 247			post_higherunit(pp);
	addu	$20, $20, -4
	.loc	2 248
 # 248		}
	.loc	2 248
	addu	$22, $22, -4
	addu	$23, $23, -4
	addu	$30, $30, -4
	lw	$24, 52($sp)
	blt	$24, $22, $49
$54:
	.loc	2 249
 # 249		amax = precb-1;
	.loc	2 250
 # 250		for (i=preca; i < preca + precb -1; i++) {
	lw	$14, 164($sp)
	move	$21, $14
	lw	$15, 72($sp)
	addu	$25, $15, -1
	sw	$25, 68($sp)
	bge	$14, $25, $60
	lw	$24, 160($sp)
	addu	$15, $24, -1
	sw	$15, 72($sp)
	negu	$2, $14
	lw	$25, 172($sp)
	mul	$24, $2, 4
	addu	$30, $25, $24
	addu	$15, $30, 8
	sw	$15, 56($sp)
	lw	$14, 176($sp)
	addu	$25, $2, $21
	negu	$24, $25
	mul	$15, $24, 4
	addu	$22, $14, $15
	addu	$23, $22, -8
$55:
	.loc	2 251
 # 251			amin = i-preca + 1;
	.loc	2 252
 # 252			ma = word_index(multiplicand,preca-2);
	lw	$13, 56($sp)
	.loc	2 253
 # 253			mb = word_index(multiplier, - preca + 2 +i) ;
	move	$16, $23
	.loc	2 254
 # 254				lmul(word_v(ma,1),word_v(mb,-1),ml, mh);
	lw	$2, 4($30)
	lw	$3, -4($22)
	multu	$3,$2
	mflo	$9
	mfhi	$10
	.loc	2 255
 # 255				al = ah+ carryl;
	addu	$4, $12, $19
	move	$8, $4
	.loc	2 256
 # 256				carryl = carryh + (al < ah);
	sltu	$25, $4, $12
	addu	$19, $18, $25
	.loc	2 257
 # 257				ah = 0;
	move	$12, $0
	.loc	2 258
 # 258				carryh = 0;
	move	$18, $0
	.loc	2 259
 # 259			j = amax - amin;
	lw	$24, 72($sp)
	lw	$14, 164($sp)
	subu	$15, $21, $14
	subu	$5, $24, $15
	addu	$5, $5, -1
	move	$31, $5
	.loc	2 260
 # 260			k = j & 3;
	.loc	2 261
 # 261			while (k--) {
	and	$6, $5, 3
	addu	$17, $6, -1
	beq	$6, $0, $57
$56:
	.loc	2 262
 # 262				lmul( *ma, *mb, nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 263
 # 263				al += ml;
	addu	$8, $8, $9
	.loc	2 264
 # 264				carryl += (al < ml);
	sltu	$25, $8, $9
	addu	$19, $19, $25
	.loc	2 265
 # 265				ah += mh;
	addu	$12, $12, $10
	.loc	2 266
 # 266				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 267
 # 267				post_lowerunit(ma);
	addu	$13, $13, 4
	.loc	2 268
 # 268				post_higherunit(mb);
	addu	$16, $16, -4
	.loc	2 269
 # 269				ml = nml;
	mflo	$9
	.loc	2 270
 # 270				mh = nmh;
	mfhi	$10
	.loc	2 271
 # 271			}
	.loc	2 271
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $56
$57:
	.loc	2 272
 # 272			k = j >> 2;
	.loc	2 273
 # 273			while (k--) {
	sra	$2, $31, 2
	addu	$17, $2, -1
	beq	$2, $0, $59
$58:
	.loc	2 274
 # 274				lmul( word_v(ma,0), word_v(mb,0), nml, nmh);
	lw	$2, 0($13)
	lw	$3, 0($16)
	multu	$2, $3
	.loc	2 275
 # 275				al += ml;
	addu	$8, $8, $9
	.loc	2 276
 # 276				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 277
 # 277				ah += mh;
	addu	$12, $12, $10
	.loc	2 278
 # 278				carryh += (ah < mh);
	sltu	$15, $12, $10
	addu	$18, $18, $15
	.loc	2 279
 # 279				ml = nml;
	mflo	$9
	.loc	2 280
 # 280				mh = nmh;
	mfhi	$10
	.loc	2 282
 # 281	
 # 282				lmul( word_v(ma,-1), word_v(mb,1), nml, nmh);
	lw	$4, 4($13)
	lw	$5, -4($16)
	multu	$4, $5
	.loc	2 283
 # 283				al += ml;
	addu	$8, $8, $9
	.loc	2 284
 # 284				carryl += (al < ml);
	sltu	$25, $8, $9
	addu	$19, $19, $25
	.loc	2 285
 # 285				ah += mh;
	addu	$12, $12, $10
	.loc	2 286
 # 286				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 287
 # 287				ml = nml;
	mflo	$9
	.loc	2 288
 # 288				mh = nmh;
	mfhi	$10
	.loc	2 290
 # 289	
 # 290				lmul( word_v(ma,-2), word_v(mb,2), nml, nmh);
	lw	$2, 8($13)
	lw	$3, -8($16)
	multu	$2, $3
	.loc	2 291
 # 291				al += ml;
	addu	$8, $8, $9
	.loc	2 292
 # 292				carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 293
 # 293				ah += mh;
	addu	$12, $12, $10
	.loc	2 294
 # 294				carryh += (ah < mh);
	sltu	$15, $12, $10
	addu	$18, $18, $15
	.loc	2 295
 # 295				ml = nml;
	mflo	$9
	.loc	2 296
 # 296				mh = nmh;
	mfhi	$10
	.loc	2 298
 # 297	
 # 298				lmul( word_v(ma,-3), word_v(mb,3), nml, nmh);
	lw	$4, 12($13)
	lw	$5, -12($16)
	multu	$4, $5
	.loc	2 299
 # 299				al += ml;
	addu	$8, $8, $9
	.loc	2 300
 # 300				carryl += (al < ml);
	sltu	$25, $8, $9
	addu	$19, $19, $25
	.loc	2 301
 # 301				ah += mh;
	addu	$12, $12, $10
	.loc	2 302
 # 302				carryh += (ah < mh);
	sltu	$14, $12, $10
	addu	$18, $18, $14
	.loc	2 303
 # 303				ml = nml;
	mflo	$9
	.loc	2 304
 # 304				mh = nmh;
	mfhi	$10
	.loc	2 306
 # 305	
 # 306				nlowerunit(ma,4);
	addu	$13, $13, 16
	.loc	2 307
 # 307				nhigherunit(mb,4);
	addu	$16, $16, -16
	.loc	2 308
 # 308			}
	.loc	2 308
	move	$11, $17
	addu	$17, $17, -1
	bne	$11, 0, $58
$59:
	.loc	2 310
 # 309	
 # 310			al += ml;
	addu	$8, $8, $9
	.loc	2 311
 # 311			carryl += (al < ml);
	sltu	$24, $8, $9
	addu	$19, $19, $24
	.loc	2 312
 # 312			*pp = al;
	sw	$8, 0($20)
	.loc	2 313
 # 313			ah += mh;
	addu	$12, $12, $10
	.loc	2 314
 # 314			carryh += (ah < mh);
	sltu	$15, $12, $10
	addu	$18, $18, $15
	.loc	2 315
 # 315			post_higherunit(pp);
	addu	$20, $20, -4
	.loc	2 316
 # 316		}
	.loc	2 316
	addu	$21, $21, 1
	addu	$22, $22, -4
	addu	$23, $23, -4
	lw	$25, 68($sp)
	blt	$21, $25, $55
$60:
	.loc	2 317
 # 317		al = ah + carryl;
	.loc	2 318
 # 318		carryh += (al < ah);
	.loc	2 319
 # 319		*pp  = al;
	addu	$14, $12, $19
	sw	$14, 0($20)
	.loc	2 320
 # 320	}
$61:
	ld	$16, 0($sp)
	ld	$18, 8($sp)
	ld	$20, 16($sp)
	ld	$22, 24($sp)
	ld	$30, 32($sp)
	addu	$sp, 168
	j	$31
	.end	p_dmul
#endif /* MIPSEB */

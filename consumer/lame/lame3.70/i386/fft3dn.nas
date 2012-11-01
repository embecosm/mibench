; back port from GOGO-no coda 2.24b by Takehiro TOMINAGA

; GOGO-no-coda
;	Copyright (C) 1999 shigeo
;	special thanks to URURI

%include "nasm.h"

	externdef costab_fft
	externdef sintab_fft

	segment_data
	align 32
D_1_41421	dd	1.41421356	, 1.41421356
D_1_0	dd	1.0		, 1.0
D_0_5	dd	0.5		, 0.5
D_0_25	dd	0.25	, 0.25
D_0_02236	dd	0.02236067	, 0.02236067
D_0_0005	dd	0.0005	, 0.0005
D_0_0	dd	0.0		, 0.0

D_1_0_D_0_0	dd	0.0		, 1.0
D_0_0_D_1_0	dd	1.0		, 0.0

D_MSB1_0	dd	0x00000000	, 0x80000000
D_MSB1_1	dd	0x80000000	, 0x80000000
D_MSB0_1	dd	0x80000000	, 0x00000000

	segment_code

;void fht_3DN2(float *fz, int n);
proc	fht_3DN2

%$fz	arg	4
%$n	arg	4

%$k	local	4

%$Ps2_Pc2	local	8
%$Mc2_Ps2	local	8

%$t_s	local	8
%$t_c	local	8
	alloc

	femms
	pushd	ebp, ebx, esi, edi

fht_3DN_1st_part:

fht_3DN_2nd_part:

fht_3DN_3rd_part:

.do_init:
	mov	r3, 16			;k1*fsize = 4*fsize = k4
	mov	r4, 8			;kx = k1/2
	mov	r2, 48			;k3*fsize
	mov	dword [sp(%$k)], 2	;k = 2
	mov	r0, [sp(%$fz)]		;fi
	lea	r1, [r0+8]		;gi = fi + kx
	jmp	.do

	align 16
.do:
	pmov	mm6, [D_MSB1_0]		;MSB1_0
	pmov	mm7, [D_1_41421]

.do2:
	;f
	pmov	mm1, [r0+r3]	;fi1
	pmov	mm4, [r0+r2]	;fi3
	pmov	mm0, [r0]		;fi0
	pmov	mm3, [r0+r3*2]	;fi2

	pupldq	mm1, mm1
	pupldq	mm4, mm4
	pupldq	mm0, mm0		;fi0 | fi0
	pupldq	mm3, mm3		;fi2 | fi2

	pxor	mm1, mm6		;-fi1 | fi1
	pxor	mm4, mm6		;-fi3 | fi3

	pfadd	mm0, mm1		;f1 | f0
	pfadd	mm3, mm4		;f3 | f2

	pmov	mm4, mm0
	pfadd	mm0, mm3		;fi1 | fi0
	pfsub	mm4, mm3		;fi3 | fi2

	pmovd	[r0], mm0		;fi[0]
	puphdq	mm0, mm0
	pmovd	[r0+r3*2], mm4	;fi[k2]
	puphdq	mm4, mm4

	pmovd	[r0+r3], mm0	;fi[k1]
	pmovd	[r0+r2], mm4	;fi[k3]

	;g
	pmov	mm1, [r1+r3]	;gi1
	pmov	mm0, [r1]		;gi0
	pmov	mm3, [r1+r3*2]	;gi2
	pmov	mm5, [r1+r2]	;gi3

	pupldq	mm1, mm1
	pupldq	mm0, mm0		;gi0 | gi0
	pupldq	mm3, mm5		;gi3 | gi2

	pxor	mm1, mm6		;-gi1 | gi1

	pfadd	mm0, mm1		;g1 | g0
	pfmul	mm3, mm7		;g3 | g2

	pmov	mm4, mm0
	pfadd	mm0, mm3		;gi1 | gi0
	pfsub	mm4, mm3		;gi3 | gi2

	pmovd	[r1], mm0		;gi[0]
	puphdq	mm0, mm0
	pmovd	[r1+r3*2], mm4	;gi[k2]
	puphdq	mm4, mm4

	pmovd	[r1+r3], mm0	;gi[k1]
	pmovd	[r1+r2], mm4	;gi[k3]

	lea	r0, [r0+r3*4]
	lea	r1, [r1+r3*4]
	cmp	r0, r6
	jb near .do2

	
	mov	r0, [sp(%$k)]
	pmov	mm0, [costab_fft +r0*4]
	pmov	mm1, [sintab_fft +r0*4]
	pupldq	mm0, mm0
	pupldq	mm1, mm1
	pmov	mm6, [D_1_0_D_0_0]		;c1 | s1
	pmov	mm7, [D_0_0_D_1_0]		;-s1 | c1
	pmov	[sp(%$t_c)], mm0
	pmov	[sp(%$t_s)], mm1

.for_init:
	mov	r5, 4		;i = 1*fsize
	jmp	.for

	align 16
.for:
	pfmul	mm6, [sp(%$t_c)]	;c1*t_c | s1*t_c
	pfmul	mm7, [sp(%$t_s)]	;-s1*t_s | c1*t_s

	pfadd	mm6, mm7		;c1 | s1
	pmov	mm7, [D_MSB0_1]

	pmov	mm1, mm6
	pxor	mm7, mm6		;c1 | -s1

	puphdq	mm1, mm1		;c1
	pmov	mm0, mm7

	pupldq	mm2, mm7
	pfmul	mm0, mm6		;c1*c1 | -s1*s1
	pfmul	mm1, mm6		;c1*s1

	puphdq	mm7, mm2		;-s1 | c1
	pfacc	mm0, mm0		;c2
	pfadd	mm1, mm1		;s2 = 2*c1*s1

	pupldq	mm1, mm0		;c2 | s2
	pupldq	mm0, mm1		;s2 | c2

	pxor	mm1, [D_MSB1_0]	;-c2 | s2

	pmov	[sp(%$Ps2_Pc2)], mm0
	pmov	[sp(%$Mc2_Ps2)], mm1

	mov	r0, [sp(%$fz)]
	mov	r1, [sp(%$fz)]
	add	r0, r5		;r0 = fi
	add	r1, r3
	sub	r1, r5		;r1 = gi
	jmp	.do3

	align 16
.do3:
	pmov	mm2, [r0+r3]
	pmov	mm4, [r1+r3]
	pmov	mm3, [r0+r2]
	pmov	mm5, [r1+r2]

	pupldq	mm2, mm2
	pupldq	mm4, mm4
	pupldq	mm3, mm3
	pupldq	mm5, mm5

	pmov	mm0, [sp(%$Ps2_Pc2)]
	pmov	mm1, [sp(%$Mc2_Ps2)]

	pfmul	mm2, mm0		;s2 * fi1 | c2 * fi1
	pfmul	mm4, mm1		;-c2 * gi1 | s2 * gi1
	pfmul	mm3, mm0		;s2 * fi3 | c2 * fi3
	pfmul	mm5, mm1		;-c2 * gi3 | s2 * gi3

	pfadd	mm2, mm4		;b | a
	pfadd	mm3, mm5		;d | c

	pmov	mm0, [r0]
	pmov	mm4, [r1]
	pmov	mm1, [r0+r3*2]
	pmov	mm5, [r1+r3*2]

	pupldq	mm0, mm4		;gi0 | fi0
	pupldq	mm1, mm5		;gi2 | fi2

	pmov	mm4, mm2
	pmov	mm5, mm3

	pfadd	mm2, mm0		;g0 | f0
	pfadd	mm3, mm1		;g2 | f2

	pfsub	mm0, mm4		;g1 | f1
	pfsub	mm1, mm5		;g3 | f3


	pmov	mm4, mm3
	pmov	mm5, mm1

	pupldq	mm4, mm4		;f2 | f2
	puphdq	mm5, mm5		;g3 | g3
	puphdq	mm3, mm3		;g2 | g2
	pupldq	mm1, mm1		;f3 | f3

	pfmul	mm4, mm6		;f2 * c1 | f2 * s1
	pfmul	mm5, mm7		;g3 * -s1 | g3 * c1
	pfmul	mm3, mm6		;g2 * c1 | g2 * s1
	pfmul	mm1, mm7		;f3 * -s1 | f3 * c1

	pfsub	mm4, mm5		;a | b
	pfadd	mm3, mm1		;d | c

	pmov	mm5, mm2
	pmov	mm1, mm0

	pupldq	mm2, mm2		;f0 | f0
	pupldq	mm0, mm0		;f1 | f1

	puphdq	mm1, mm2		;f0 | g1
	puphdq	mm5, mm0		;f1 | g0

	pmov	mm2, mm4
	pmov	mm0, mm3

	pfadd	mm4, mm1		;fi0 | gi1
	pfadd	mm3, mm5		;fi1 | gi0
	pfsub	mm1, mm2		;fi2 | gi3
	pfsub	mm5, mm0		;fi3 | gi2

	pmovd	[r1+r3], mm4	;gi[k1]
	puphdq	mm4, mm4
	pmovd	[r1], mm3		;gi[0]
	puphdq	mm3, mm3
	pmovd	[r1+r2], mm1	;gi[k3]
	puphdq	mm1, mm1
	pmovd	[r1+r3*2], mm5	;gi[k2]
	puphdq	mm5, mm5

	pmovd	[r0], mm4		;fi[0]
	pmovd	[r0+r3], mm3	;fi[k1]
	pmovd	[r0+r3*2], mm1	;fi[k2]
	pmovd	[r0+r2], mm5	;fi[k3]

	lea	r0, [r0+r3*4]
	lea	r1, [r1+r3*4]
	cmp	r0, r6
	jb near	.do3

	add	r5, 4
	cmp	r5, r4
	jb near	.for

	cmp	r3, [sp(%$n)]
	jae	.exit

	add	dword [sp(%$k)], 2	;k  += 2;
	lea	r3, [r3*4]		;k1 *= 4
	lea	r2, [r2*4]		;k3 *= 4
	lea	r4, [r4*4]		;kx *= 4
	mov	r0, [sp(%$fz)]	;fi
	lea	r1, [r0+r4]		;gi = fi + kx
	jmp	.do

.exit:
	femms
	popd	ebp, ebx, esi, edi
endproc

;***********************************************************************
%ifdef USE_E3DN

;void fht_E3DN(float *fz, int n);
proc	fht_E3DN

%$fz	arg	4
%$n	arg	4

%$k	local	4

%$Ps2_Pc2	local	8
%$Mc2_Ps2	local	8

%$t_s	local	8
%$t_c	local	8
	alloc

	femms
	pushd	ebp, ebx, esi, edi

fht_E3DN_1st_part:

fht_E3DN_2nd_part:

fht_E3DN_3rd_part:

.do_init:
	mov	r3, 16			;k1*fsize = 4*fsize = k4
	mov	r4, 8			;kx = k1/2
	mov	r2, 48			;k3*fsize
	mov	dword [sp(%$k)], 2	;k = 2
	mov	r0, [sp(%$fz)]		;fi
	lea	r1, [r0+8]		;gi = fi + kx
	jmp	.do

	align 16
.do:
	pmov	mm7, [D_1_41421]

.do2:
	pmov	mm0, [r0]		;fi0
	pupldq	mm0, [r0+r3]	;fi1 | fi0
	pmov	mm1, [r0+r3*2]	;fi2
	pupldq	mm1, [r0+r2]	;fi3 | fi2
	pmov	mm3, [r1]		;gi0
	pupldq	mm3, [r1+r3]	;gi1 | gi0
	pmov	mm4, [r1+r2]	;gi3
	pupldq	mm4, [r1+r3*2]	;gi2 | gi3

	pfpnacc	mm0, mm0		;f0 | f1
	pfpnacc	mm1, mm1		;f2 | f3
	pfpnacc	mm3, mm3		;g0 | g1
	pfmul	mm4, mm7		;g2 | g3

	pmov	mm2, mm0
	pfadd	mm0, mm1		;fi0 | fi1
	pfsub	mm2, mm1		;fi2 | fi3
	pmov	mm5, mm3
	pfadd	mm3, mm4		;gi0 | gi1
	pfsub	mm5, mm4		;gi2 | gi3

	pmovd	[r0+r3], mm0	;fi[k1]
	puphdq	mm0, mm0
	pmovd	[r0+r2], mm2	;fi[k3]
	puphdq	mm2, mm2
	pmovd	[r1+r3], mm3	;gi[k1]
	puphdq	mm3, mm3
	pmovd	[r1+r2], mm5	;gi[k3]
	puphdq	mm5, mm5

	pmovd	[r0], mm0		;fi[0]
	pmovd	[r0+r3*2], mm2	;fi[k2]
	pmovd	[r1], mm3		;gi[0]
	pmovd	[r1+r3*2], mm5	;gi[k2]

	lea	r0, [r0+r3*4]
	lea	r1, [r1+r3*4]
	cmp	r0, r6
	jb near .do2

	
	mov	r0, [sp(%$k)]
	pmov	mm0, [costab_fft +r0*4]
	pmov	mm1, [sintab_fft +r0*4]
	pupldq	mm0, mm0
	pupldq	mm1, mm1
	pmov	mm6, [D_1_0_D_0_0]		;c1 | s1
	pmov	mm7, [D_0_0_D_1_0]		;-s1 | c1
	pmov	[sp(%$t_c)], mm0
	pmov	[sp(%$t_s)], mm1

.for_init:
	mov	r5, 4		;i = 1*fsize
	jmp	.for

	align 16
.for:
	pfmul	mm6, [sp(%$t_c)]	;c1*t_c | s1*t_c
	pfmul	mm7, [sp(%$t_s)]	;-s1*t_s | c1*t_s

	pfadd	mm6, mm7		;c1 | s1
	pmov	mm7, [D_MSB0_1]

	pswapd	mm1, mm6		;s1 | c1
	pswapd	mm0, mm6
	pxor	mm7, mm6		;c1 | -s1

	pfmul	mm1, mm6		;c1*s1 | c1*s1
	pfmul	mm0, mm0		;s1*s1 | c1*c1
	pswapd	mm7, mm7		;-s1 | c1

	pfpnacc	mm0, mm1		;s2 = 2*c1*s1 | c2 = c1*c1-s1*s1
	pswapd	mm1, mm0		;c2 | s2
	pxor	mm1, [D_MSB1_0]	;-c2 | s2

	pmov	[sp(%$Ps2_Pc2)], mm0
	pmov	[sp(%$Mc2_Ps2)], mm1

	mov	r0, [sp(%$fz)]
	mov	r1, [sp(%$fz)]
	add	r0, r5		;r0 = fi
	add	r1, r3
	sub	r1, r5		;r1 = gi
	jmp	.do3

	align 16
.do3:
	pmov	mm0, [r0+r2]
	pmov	mm2, [r1+r2]
	pmov	mm1, [r0+r3]
	pmov	mm3, [r1+r3]

	pupldq	mm0, mm0
	pupldq	mm2, mm2
	pupldq	mm1, mm1
	pupldq	mm3, mm3

	pmov	mm4, [sp(%$Ps2_Pc2)]
	pmov	mm5, [sp(%$Mc2_Ps2)]

	pfmul	mm0, mm4		;s2 * fi3 | c2 * fi3
	pfmul	mm2, mm5		;-c2 * gi3 | s2 * gi3
	pfmul	mm1, mm4		;s2 * fi1 | c2 * fi1
	pfmul	mm3, mm5		;-c2 * gi1 | s2 * gi1

	pfadd	mm0, mm2		;d | c
	pfadd	mm1, mm3		;b | a

	pmov	mm2, [r0+r3*2]	;fi2
	pupldq	mm3, [r1+r3*2]	;gi2 | -
	pmov	mm4, [r0]		;fi0
	pupldq	mm5, [r1]		;gi0 | -

	pupldq	mm2, mm0		;c | fi2
	puphdq	mm3, mm0		;d | gi2
	pupldq	mm4, mm1		;a | fi0
	puphdq	mm5, mm1		;b | gi0

	pfpnacc	mm2, mm2		;f2 | f3
	pfpnacc	mm3, mm3		;g2 | g3
	pfpnacc	mm4, mm4		;f0 | f1
	pfpnacc	mm5, mm5		;g0 | g1

	pmov	mm0, mm2
	pmov	mm1, mm3
	pupldq	mm2, mm2		;f3 | f3
	pupldq	mm3, mm3		;g3 | g3
	puphdq	mm0, mm0		;f2 | f2
	puphdq	mm1, mm1		;g2 | g2

	pswapd	mm4, mm4		;f1 | f0
	pswapd	mm5, mm5		;g1 | g0

	pfmul	mm0, mm7		;f2 *-s1 | f2 * c1
	pfmul	mm3, mm6		;g3 * c1 | g3 * s1
	pfmul	mm1, mm6		;g2 * c1 | g2 * s1
	pfmul	mm2, mm7		;f3 *-s1 | f3 * c1

	pfadd	mm0, mm3		;-b | a
	pfadd	mm1, mm2		; d | c

	pmov	mm2, mm5
	pmov	mm3, mm4
	pupldq	mm4, mm0		; a | f0
	pupldq	mm5, mm1		; c | g0
	puphdq	mm2, mm0		;-b | g1
	puphdq	mm3, mm1		; d | f1

	pfpnacc	mm4, mm4		;fi0 | fi2
	pfpnacc	mm5, mm5		;gi0 | gi2
	pfpnacc	mm2, mm2		;gi3 | gi1
	pfpnacc	mm3, mm3		;fi1 | fi3

	pmovd	[r0+r3*2], mm4	;fi[k2]
	puphdq	mm4, mm4
	pmovd	[r1+r3*2], mm5	;gi[k2]
	puphdq	mm5, mm5
	pmovd	[r1+r3], mm2	;gi[k1]
	puphdq	mm2, mm2
	pmovd	[r0+r2], mm3	;fi[k3]
	puphdq	mm3, mm3

	pmovd	[r0], mm4		;fi[0]
	pmovd	[r1], mm5		;gi[0]
	pmovd	[r1+r2], mm2	;gi[k3]
	pmovd	[r0+r3], mm3	;fi[k1]

	lea	r0, [r0+r3*4]
	lea	r1, [r1+r3*4]
	cmp	r0, r6
	jb near	.do3

	add	r5, 4
	cmp	r5, r4
	jb near	.for

	cmp	r3, [sp(%$n)]
	jae	.exit

	add	dword [sp(%$k)], 2	;k  += 2;
	lea	r3, [r3*4]		;k1 *= 4
	lea	r2, [r2*4]		;k3 *= 4
	lea	r4, [r4*4]		;kx *= 4
	mov	r0, [sp(%$fz)]	;fi
	lea	r1, [r0+r4]		;gi = fi + kx
	jmp	.do

.exit:
	femms
	popd	ebp, ebx, esi, edi
endproc

%endif

;***********************************************************************

;void fft_side_3DN(float in[2][1024], int s, float *ret); /* s = MSFREQ は4の倍数を仮定。作成時は20だった */
proc fft_side_3DN

%$in	arg	4
%$s	arg	4
%$pret	arg	4

	femms
	pushd	ebx, esi

.for_init:
	mov	r0, [sp(%$in)]		;r0 = &in[0][0]
	lea	r1, [r0+fsizen(1024)]	;r1 = &in[1][0]

	mov	r2, [sp(%$s)]
	mov	r3, fsizen(1023)
	shl	r2, 2			;r2 = s * fsize
	sub	r3, r2			;r3 = (1023-s) * fsize

	mov	r4, fsizen(512)		;r4 = 512 * fsize
	pxor	mm7, mm7
	jmp	.for

	align 16
.for:
	pmov	mm0, [r0+r3]
	pmov	mm1, [r1+r3]
	pmov	mm2, [r0+r2]
	pfsub	mm0, mm1
	pmov	mm3, [r1+r2]
	pfsub	mm2, mm3
	pmov	mm4, [r0+r3-fsizen(2)]
	pfmul	mm0, mm0
	pmov	mm5, [r1+r3-fsizen(2)]
	pfmul	mm2, mm2
	pmov	mm1, [r0+r2+fsizen(2)]
	pupldq	mm6, mm0
	pmov	mm3, [r1+r2+fsizen(2)]
	pfadd	mm7, mm2
	pfsub	mm4, mm5
	puphdq	mm0, mm6
	pfsub	mm1, mm3
	pfadd	mm7, mm0

	pfmul	mm4, mm4
	add	r2, fsizen(4)
	pfmul	mm1, mm1
	sub	r3, fsizen(4)
	pupldq	mm6, mm4
	pfadd	mm7, mm1
	cmp	r2, r4
	puphdq	mm4, mm6
	pfadd	mm7, mm4
	jb	.for

	pmov	mm0, [r0+r2]
	pmov	mm1, [r1+r2]
	pmov	mm2, [D_0_25]
	pfsub	mm0, mm1
	pmov	mm3, [D_0_5]
	pfacc	mm7, mm7
	pfmul	mm0, mm0
	pfmul	mm7, mm2
	pfmul	mm0, mm3
	mov	r0, [sp(%$pret)]
	pfadd	mm0, mm7

	pmovd	[r0], mm0
.exit:
	femms
	popd	ebx, esi
endproc

;***********************************************************************
%ifdef USE_E3DN

;void fft_side_E3DN(float in[2][1024], int s, float *ret); /* s = MSFREQ は4の倍数を仮定。作成時は20だった */
proc fft_side_E3DN

%$in	arg	4
%$s	arg	4
%$pret	arg	4

	femms
	pushd	ebx, esi

.for_init:
	mov	r0, [sp(%$in)]		;r0 = &in[0][0]
	lea	r1, [r0+fsizen(1024)]	;r1 = &in[1][0]

	mov	r2, [sp(%$s)]
	mov	r3, fsizen(1023)
	shl	r2, 2			;r2 = s * fsize
	sub	r3, r2			;r3 = (1023-s) * fsize

	mov	r4, fsizen(512)		;r4 = 512 * fsize
	pxor	mm7, mm7
	jmp	.for

	align 16
.for:
	pmov	mm0, [r0+r3]
	pfsub	mm0, [r1+r3]
	pmov	mm4, [r0+r3-fsizen(2)]
	pfsub	mm4, [r1+r3-fsizen(2)]
	pmov	mm2, [r0+r2]
	pfsub	mm2, [r1+r2]
	pmov	mm1, [r0+r2+fsizen(2)]
	pfsub	mm1, [r1+r2+fsizen(2)]
	pfmul	mm0, mm0
	pfmul	mm4, mm4
	pfmul	mm2, mm2
	pfmul	mm1, mm1
	pswapd	mm0, mm0
	pswapd	mm4, mm4
	pfadd	mm7, mm2
	add	r2, fsizen(4)
	pfadd	mm7, mm1
	sub	r3, fsizen(4)
	pfadd	mm7, mm0
	cmp	r2, r4
	pfadd	mm7, mm4
	jb	.for

	pmov	mm0, [r0+r2]
	pfsub	mm0, [r1+r2]
	pfacc	mm7, mm7
	pfmul	mm0, mm0
	pfmul	mm7, [D_0_25]
	pfmul	mm0, [D_0_5]
	mov	r0, [sp(%$pret)]
	pfadd	mm0, mm7

	pmovd	[r0], mm0
.exit:
	femms
	popd	ebx, esi
endproc

%endif

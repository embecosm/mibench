;
; Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, and Jean-loup Gailly.
; Permission is granted to any individual or institution to use, copy, or
; redistribute this software so long as all of the original files are included
; unmodified, that it is not sold for profit, and that this copyright notice
; is retained.
;
; match.asm by Jean-loup Gailly.

; match.asm, optimized version of longest_match() in deflate.c
; Must be assembled with masm -ml. To be used only with C large model.
; (For compact model, follow the instructions given below.)
; This file is only optional. If you don't have masm or tasm, use the
; C version (add -DNO_ASM to CFLAGS in makefile.msc and remove match.obj
; from OBJI). If you have reduced WSIZE in zip.h, then change its value
; below.
;
; Turbo C 2.0 does not support static allocation of more than 64K bytes per
; file, and does not have SS == DS. So TC and BC++ users must use:
;   tasm -ml -DDYN_ALLOC -DSS_NEQ_DS match;
;
; To simplify the code, the option -DDYN_ALLOC is supported for OS/2
; only if the arrays are guaranteed to have zero offset (allocated by
; halloc). We also require SS==DS. This is satisfied for MSC but not Turbo C.

        name    match

; define LCODE as follows:
; model:        compact large (small and medium not supported here)
; LCODE           0       1

LCODE equ 1
; Better define them on the command line
;DYN_ALLOC equ 1
;SS_NEQ_DS equ 1
; For Turbo C, define SS_NEQ_DS as 1, but for MSC you can leave it undefined.
; The code is a little better when SS_NEQ_DS is not defined.

ifndef DYN_ALLOC
        extrn   _prev         : word
        extrn   _slide        : byte
        prev    equ  _prev    ; offset part
        slide   equ  _slide
endif

_DATA    segment  word public 'DATA'
        extrn   _match_start  : word
        extrn   _prev_length  : word
        extrn   _good_match   : word
        extrn   _strstart     : word
        extrn   _max_chain_length : word
ifdef DYN_ALLOC
        extrn   _prev         : word
        extrn   _slide        : word
        prev    equ 0         ; offset forced to zero
        slide   equ 0
        slide_seg equ _slide[2]
	slide_off equ 0
else
	wseg    dw seg _slide
        slide_seg equ wseg
	slide_off equ offset _slide
endif
_DATA    ends

DGROUP  group _DATA

if LCODE
	extrn   _exit : far
else
	extrn   _exit : near
endif

_TEXT   segment word public 'CODE'
        assume  cs: _TEXT, ds: DGROUP

	public _match_init
        public _longest_match

	MIN_MATCH     equ 3
        MAX_MATCH     equ 258
	WSIZE         equ 8192		; keep in sync with zip.h !
	WMASK         equ (WSIZE-1)
	MIN_LOOKAHEAD equ (MAX_MATCH+MIN_MATCH+1)
	MAX_DIST      equ (WSIZE-MIN_LOOKAHEAD)

prev_ptr    dw  seg _prev		; pointer to the prev array
ifdef SS_NEQ_DS
    match_start dw  0			; copy of _match_start if SS != DS
endif

; initialize or check the variables used in match.asm.

if LCODE
_match_init proc far
else
_match_init proc near
endif
ifdef SS_NEQ_DS
        ma_start equ cs:match_start	; does not work on OS/2
else
	assume ss: DGROUP
        ma_start equ ss:_match_start
        mov     ax,ds
        mov     bx,ss
        cmp     ax,bx                   ; SS == DS?
        jne     error
endif
ifdef DYN_ALLOC
	mov	ax,_slide[0]		; force zero offset
	add     ax,15
	mov     cx,4
	shr     ax,cl
	add     _slide[2],ax
	mov     _slide[0],0

	mov	ax,_prev[0]		; force zero offset
	add     ax,15
	mov     cx,4
	shr     ax,cl
	add     _prev[2],ax
	mov     _prev[0],0
  ifdef SS_NEQ_DS
	mov	ax,_prev[2]		; segment value
	mov     cs:prev_ptr,ax		; ugly write to code, crash on OS/2
        prev_seg  equ cs:prev_ptr
  else
        prev_seg  equ ss:_prev[2]	; works on OS/2 if SS == DS
  endif
else
        prev_seg  equ cs:prev_ptr
endif
	ret
error:  call    _exit

_match_init endp

; -----------------------------------------------------------------------
; Set match_start to the longest match starting at the given string and
; return its length. Matches shorter or equal to prev_length are discarded,
; in which case the result is equal to prev_length and match_start is
; garbage.
; IN assertions: cur_match is the head of the hash chain for the current
;   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1

; int longest_match(cur_match)

if LCODE
_longest_match  proc far
else
_longest_match  proc near
endif
        push    bp
        mov     bp,sp
        push    di
	push	si
	push	ds

if LCODE
        cur_match    equ word ptr [bp+6]
else
        cur_match    equ word ptr [bp+4]
endif
;       slide	     equ es:slide (es:0 for DYN_ALLOC)
;       prev	     equ ds:prev
;       match        equ es:si
;       scan         equ es:di
;       chain_length equ bp
;       best_len     equ bx
;       limit        equ dx

	mov	si,cur_match            ; use bp before it is destroyed
        mov     bp,_max_chain_length    ; chain_length = max_chain_length
	mov	di,_strstart
	mov	dx,di
	sub	dx,MAX_DIST             ; limit = strstart-MAX_DIST
	jae	limit_ok
	sub	dx,dx			; limit = NIL
limit_ok:
        add     di,2+slide_off          ; di = offset(slide + strstart + 2)
        mov     bx,_prev_length         ; best_len = prev_length
	mov     es,slide_seg
        mov     ax,es:[bx+di-3]         ; ax = scan[best_len-1..best_len]
        mov     cx,es:[di-2]            ; cx = scan[0..1]
	cmp	bx,_good_match		; do we have a good match already?
        mov     ds,prev_seg    		; (does not destroy the flags)
        assume  ds: nothing
        jb      do_scan			; good match?
	shr	bp,1			; chain_length >>= 2
	shr	bp,1
        jmp     short do_scan

        even                            ; align destination of branch
long_loop:
; at this point, ds:di == scan+2, ds:si == cur_match
        mov     ax,[bx+di-3]            ; ax = scan[best_len-1..best_len]
        mov     cx,[di-2]               ; cx = scan[0..1]
        mov     ds,prev_seg    		; reset ds to address the prev array
short_loop:
        dec     bp                      ; --chain_length
        jz      the_end
; at this point, di == scan+2, si = cur_match,
; ax = scan[best_len-1..best_len] and cx = scan[0..1]
if (WSIZE-32768)
        and     si,WMASK
endif
        shl     si,1                    ; cur_match as word index
        mov     si,prev[si]             ; cur_match = prev[cur_match]
        cmp     si,dx			; cur_match <= limit ?
        jbe     the_end
do_scan:
        cmp     ax,word ptr es:slide[bx+si-1] ; check match at best_len-1
        jne     short_loop
        cmp     cx,word ptr es:slide[si]      ; check min_match_length match
        jne     short_loop

        lea     si,slide[si+2]          ; si = match
        mov     ax,di                   ; ax = scan+2
        mov     cx,es
        mov     ds,cx			; ds = es = slide
        mov     cx,(MAX_MATCH-2)/2      ; scan for at most MAX_MATCH bytes
        repe    cmpsw                   ; loop until mismatch
        je      maxmatch                ; match of length MAX_MATCH?
mismatch:
        mov     cl,[di-2]               ; mismatch on first or second byte?
        sub     cl,[si-2]               ; cl = 0 if first bytes equal
        xchg    ax,di                   ; di = scan+2, ax = end of scan
        sub     ax,di                   ; ax = len
	sub	si,ax			; si = cur_match + 2 + offset(slide)
	sub	si,2+slide_off          ; si = cur_match
        sub     cl,1                    ; set carry if cl == 0 (can't use DEC)
        adc     ax,0                    ; ax = carry ? len+1 : len
        cmp     ax,bx                   ; len > best_len ?
        jle     long_loop
        mov     ma_start,si             ; match_start = cur_match
        mov     bx,ax                   ; bx = best_len = len
        cmp     ax,MAX_MATCH            ; len >= MAX_MATCH ?
        jl      long_loop
the_end:
	pop	ds
        assume  ds: DGROUP
ifdef SS_NEQ_DS
	mov	ax,ma_start		; garbage if no match found
	mov	ds:_match_start,ax
endif
        pop     si
        pop     di
        pop     bp
        mov     ax,bx                   ; result = ax = best_len
        ret
maxmatch:                               ; come here if maximum match
        cmpsb                           ; increment si and di
        jmp     mismatch                ; force match_length = MAX_LENGTH
        
_longest_match  endp

_TEXT   ends
end

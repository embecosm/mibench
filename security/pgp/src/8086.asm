;  Assembly primitives for RSA multiprecision library
;
;  Tested with Turbo Assembler 1.0 and masm 1.00
;
;  Written by Branko Lankester (lankeste@fwi.uva.nl)    10/10/91
;
;  Modified to add, rather than store carry bit to allow using a
;  smaller precision for long division.

; define LDATA and LCODE as follows:
; model:        small   compact medium  large
; LDATA         0       1       0       1
; LCODE         0       0       1       1

LDATA   equ     1
LCODE   equ     1

; Note: Only the large memory model has been implemented for P_SMULA,
; P_SETRECIP and P_QUO_DIGIT.

IF LDATA
DSTPTR  equ     es:[bx+si]
ELSE
DSTPTR  equ     [bx+si]
ENDIF

IF LCODE
prec    equ     [bp+6]          ; 1st arg
r1      equ     [bp+6]          ; 1st arg
IF LDATA
r2      equ     [bp+10]         ; 2nd arg
carry   equ     [bp+14]         ; 3rd arg
scarry  equ     [bp+10]         ; carry for shift (arg 2)
ELSE
r2      equ     [bp+8]
carry   equ     [bp+10]
scarry  equ     [bp+8]
ENDIF
ELSE                            ; small code model
prec    equ     [bp+4]
r1      equ     [bp+4]
IF LDATA
r2      equ     [bp+8]
carry   equ     [bp+12]
scarry  equ     [bp+8]
ELSE
r2      equ     [bp+6]
carry   equ     [bp+8]
scarry  equ     [bp+6]
ENDIF
ENDIF

IF NOT LCODE
UPTON_TEXT = _TEXT
ENDIF

_TEXT   segment byte public 'CODE'
DGROUP  group   _DATA,_BSS
        assume  cs:_TEXT,ds:DGROUP
_TEXT   ends

_DATA   segment word public 'DATA'
_DATA   ends

_BSS    segment word public 'BSS'
prec16  dw      ?               ; precision / 16 (seems to be / 256?)
unitprec dw     ?               ; precision / 16, really
addp    dw      ?               ; jump offset
subp    dw      ?
rotp    dw      ?
mulp    dw      ?
_BSS    ends

_TEXT   segment byte public 'CODE'

        public  _P_SETP
        public  _P_ADDC
        public  _P_SUBB
        public  _P_MUSUBB
        public  _P_ROTL

IF LCODE
fprims  proc    far                     ; dummy proc
ELSE
fprims  proc    near
ENDIF

;
; ******************** set precision ********************
;
_P_SETP:
        push    bp
        mov     bp,sp
        mov     ax, prec        ; precision in bits
        add     ax, 0fh
        mov     cl,4
        shr     ax,cl           ; prec. in units
        mov     unitprec,ax
        push    ax
        shr     ax,cl
        mov     prec16,ax       ; precision / 16
        pop     ax
        and     ax,0fh          ;   al = prec % 16
        mov     bx,ax
        mov     cx,ax
        shl     bx,1            ; multiply by 4 (=number of bytes
        shl     bx,1            ;   in instruction sequence)
        mov     dx,bx
IFE LDATA
        sub     dx,ax           ; small model only 3 for add/sub
ENDIF
        mov     ax,offset add_ref
        sub     ax,dx
        mov     addp,ax

        mov     ax,offset sub_ref
        sub     ax,dx
        mov     subp,ax

        mov     ax,offset rot_ref
        sub     ax,bx
        mov     rotp,ax

        mov     ax,offset mul_ref
        shl     bx,1            ; MULU macro is 17 bytes for large data
        shl     bx,1
        sub     ax,bx
        sub     ax,cx
        mov     mulp,ax

        pop     bp
        ret



;
; ******************** mpi add with carry ********************
;
ADDU    macro   n
        rept    n
                lodsw
                adc     DSTPTR,ax
        endm
endm


_P_ADDC:
        push    bp
        mov     bp,sp
        push    si
        mov     cx, prec16
        mov     dx, addp
IF LDATA
        push    ds
        lds     si, dword ptr r2
        les     bx, dword ptr r1
ELSE
        mov     si, r2
        mov     bx, r1
ENDIF
        sub     bx, si          ; calculate relative offset
        dec     bx
        dec     bx
        cld
        shr     byte ptr carry,1        ; load carry
        jcxz    add_units
add_16u:
        ADDU    16
        loop    add_16u
add_units:
        jmp     dx
        ADDU    15
add_ref:
        rcl     ax,1            ; return carry
        and     ax,1
IF LDATA
        pop     ds
ENDIF
        pop     si
        pop     bp
        ret



;
; ******************** mpi subtract with borrow ********************
;
SUBU    macro   n
        rept    n
                lodsw
                sbb     DSTPTR,ax
        endm
endm


_P_MUSUBB:              ; MULTUNIT is same size as unit
_P_SUBB:
        push    bp
        mov     bp,sp
        push    si
        mov     cx, prec16
        mov     dx, subp
IF LDATA
        push    ds
        lds     si, dword ptr r2
        les     bx, dword ptr r1
ELSE
        mov     si, r2
        mov     bx, r1
ENDIF
        sub     bx, si          ; calculate relative offset
        dec     bx
        dec     bx
        cld
        shr     byte ptr carry,1
        jcxz    sub_units
sub_16u:
        SUBU    16
        loop    sub_16u
sub_units:
        jmp     dx
        SUBU    15
sub_ref:
        rcl     ax,1            ; return carry
        and     ax,1
IF LDATA
        pop     ds
ENDIF
        pop     si
        pop     bp
        ret



;
; ******************** mpi rotate left ********************
;
_P_ROTL:
        push    bp
        mov     bp,sp
        mov     cx, prec16
        mov     dx, rotp
IF LDATA
        push    ds
        lds     bx, dword ptr r1
ELSE
        mov     bx, r1
ENDIF
        shr     byte ptr scarry,1
        jcxz    rot_units
rot_16u:
        i = 0
        rept    16
                rcl     word ptr [bx + i],1
                i = i + 2
        endm
        lahf
        add     bx,32
        sahf
        loop    rot_16u
rot_units:
        jmp     dx
        rept    15
                rcl     word ptr [bx],1
                inc     bx
                inc     bx
        endm
rot_ref:

        rcl     ax,1
        and     ax,1
IF LDATA
        pop     ds
ENDIF
        pop     bp
        ret

fprims  endp




; ***************************************************************
;  P_SMULA (MULTUNIT *prod, MULTUNIT *multiplicand, MULTUNIT multiplier)
;       mp_smul routine from Upton's modmult, converted to assembler
;
;       Multiply the single-word multiplier times the multiprecision integer 
;       in multiplicand, accumulating result in prod.  The resulting 
;       multiprecision prod will be 1 word longer than the multiplicand.   
;       multiplicand is unit_prec words long.  We add into prod, so caller 
;       should zero it out first.
;
;       NOTE:  Unlike other functions in the multiprecision arithmetic 
;       library, both multiplicand and prod are pointing at the LSB, 
;       regardless of byte order of the machine.  On an 80x86, this makes 
;       no difference.  But if this assembly function is implemented
;       on a 680x0, it becomes important.
;
;       This version differs from P_SMUL by adding in, rather than storing,
;       the final carry.  This better supports use by Smith's modmult.
; ***************************************************************
;   Variable assignments:
;       multiplier = [bp+14]
;       multiplicand = [ds:di]  32-bit pointer
;       prod = [es:si]          32-bit pointer
;       unit_prec = cx
;       p = ax-dx
;       carry = bx

        PUBLIC  _P_SMULA

MULU    macro   n
        rept    n
                lodsw                   ;multiplicand
                mul     bp              ;multiplier, results (p) to AX/DX
                add     ax,bx           ;carry
                adc     dx,0
                add     ax,WORD PTR es:[di]
                adc     dx,0
                mov     bx,dx           ;carry
                stosw
        endm
endm

_P_SMULA PROC FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        push    ds
        mov     cx,prec16
        mov     ax,mulp
        push    ax

        sub     bx,bx           ;carry = 0, store in bx

        les     di,DWORD PTR [bp+6]     ;prod in es:di
        lds     si,DWORD PTR [bp+10]    ;multiplicand in ds:si
        cld
        mov     bp,[bp+14]

        or      cx,cx
        jnz     mul_16u
        jmp     mul_units
mul_16u:
        MULU    16
        dec     cx
        jz      mul_units
        jmp     mul_16u
mul_units:
        pop     cx
        jmp     cx
        MULU    15
mul_ref:

        add     WORD PTR es:[di],bx     ;add final carry

        pop     ds
        pop     si
        pop     di
        pop     bp
        ret
_P_SMULA ENDP

; ***************************************************************
; void P_SETRECIP (MULTUNIT reciph, MULTUNIT recipl, short mshift)
;       Specify reciprocal factors for use by P_QUO_DIGIT.
;
;       This implementation is for 16-bit MULTUNIT.
;
; ***************************************************************

DGROUP  group   _DATA,_BSS
        assume  ds:DGROUP
_BSS    segment word public 'BSS'
reciph  dw      ?               ; recip msw
recipl  dw      ?               ; recip lsw
mshift  dw      ?               ; shift adjust
_BSS    ends

        PUBLIC  _P_SETRECIP

_P_SETRECIP PROC FAR
        push    bp
        mov     bp,sp

        mov     ax,6[bp]        ; reciph
        mov     reciph,ax
        mov     ax,8[bp]        ; recipl
        mov     recipl,ax
        mov     ax,10[bp]       ; mshift
        mov     mshift,ax

        pop     bp
        ret
_P_SETRECIP endp

; ***************************************************************
; MULTUNIT quo_digit (MULTUNIT *dividend)
;       Determine the next quotient digit.
;       (routine for modmult, converted to assembler)
;
;       This implementation is for 16-bit MULTUNIT.
;
;       The following items have already been set by calling
;       P_SETRECIP:
;       reciph, recipl - reciprocal of divisor
;       mshift         - scaling factor
;
;       The dividend parameter points to the most significant word
;       of the dividend.
;
; ***************************************************************
;   Register assignments:
;       dx:ax = product
;       cx:bx = temp long
;       es:si = dividend pointer
;       di    = MS word of q0
;       bp    = lsb factor
;
;   Comments reference the C implementation variables.

DGROUP  group   _DATA,_BSS
        assume  ds:DGROUP

        PUBLIC  _P_QUO_DIGIT


_P_QUO_DIGIT PROC FAR
        push    bp
        mov     bp,sp
        push    di
        push    si

        les     si,6[bp]        ; dividend
        mov     ax,es:[si-4]    ; dividend[-2]
        not     ax
        mul     reciph
        add     ax,reciph
        adc     dx,0
        mov     bx,ax
        mov     di,dx           ; di:bx = q1

        mov     ax,es:[si-2]    ; dividend[-1]
        not     ax
        mul     recipl
        inc     dx              ; dx:ax = q2

        mov     bp,dx
        and     bp,di
        and     bp,1            ; bp = lsb_factor

        add     ax,bx
        adc     di,dx
        rcr     di,1            ; di = MS word of q0

        mov     ax,es:[si-2]    ; dividend [-1]
        not     ax
        mul     reciph
        mov     bx,ax
        mov     cx,dx           ; cx:bx = q1

        mov     ax,es:[si]      ; dividend[0]
        not     ax
        mul     recipl          ; dx:ax = q2
        xor     ax,bx
        and     bp,ax           ; lsb correction
        xor     ax,bx           ; restore ax

        add     ax,bx
        adc     dx,cx
        rcr     dx,1
        rcr     ax,1            ; dx:ax = q

        add     ax,di           ; + scaled q0
        adc     dx,0
        add     ax,bp           ; + lsb correction
        adc     dx,0            ; q

        shl     ax,1
        rcl     dx,1
        rcl     ax,1
        rcl     dx,1
        rcl     ax,1
        and     ax,3
        mov     cx,ax
        mov     bx,dx           ; bx:cx = q >> 14

        mov     ax,es:[si]      ; dividend[0]
        not     ax
        mul     reciph
        shl     ax,1
        rcl     dx,1
        add     ax,bx
        adc     dx,cx           ; q

        mov     cx,mshift
        shr     ax,cl
        mov     bx,dx
        shr     dx,cl
        neg     cx
        add     cx,16
        shl     bx,cl
        add     ax,bx           ; dx:ax = q >> mshift

        or      dx,dx
        jz      no_overflow
        mov     ax,0ffffh
no_overflow:
        pop     si
        pop     di
        pop     bp
        ret
_P_QUO_DIGIT ENDP
_TEXT   ends

        end


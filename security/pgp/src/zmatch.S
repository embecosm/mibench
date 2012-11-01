/*
 * Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 * Kai Uwe Rommel and Igor Mandrichenko.
 * Permission is granted to any individual or institution to use, copy, or
 * redistribute this software so long as all of the original files are included
 * unmodified, that it is not sold for profit, and that this copyright notice
 * is retained.
 *
 * match.s by Jean-loup Gailly. Translated to 32 bit code by Kai Uwe Rommel.

 * match.s, optimized version of longest_match() in deflate.c
 * This version is for 386 Unix or OS/2 in 32 bit mode.
 * Warning: it uses the AT&T syntax: mov source,dest
 * This file is only optional. If you want to force the C version,
 * add -DNO_ASM to CFLAGS in makefile and remove match.o from OBJI).
 * If you have reduced WSIZE in zip.h, then change its value below.
 * This version assumes static allocation of the arrays (-DDYN_ALLOC not used).
 */

        .file   "zmatch.S"

#if defined(SYSV) || defined(__ELF__)
#  define _prev             prev
#  define _slide            slide
#  define _match_start      match_start
#  define _prev_length      prev_length
#  define _good_match       good_match
#  define _strstart         strstart
#  define _max_chain_length max_chain_length

#  define _match_init       match_init
#  define _longest_match    longest_match
#endif

#define MAX_MATCH       258
#define MAX_MATCH2      128    /* MAX_MATCH/2-1 */
#define MIN_MATCH       3
#define WSIZE           8192
#define MAX_DIST        WSIZE - MAX_MATCH - MIN_MATCH - 1

        .globl  _match_init
        .globl  _longest_match

        .text

_match_init:
        ret

/* -----------------------------------------------------------------------
 * Set match_start to the longest match starting at the given string and
 * return its length. Matches shorter or equal to prev_length are discarded,
 * in which case the result is equal to prev_length and match_start is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 *   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
 */

_longest_match: /* int longest_match(cur_match) */

#define cur_match   20(%esp)
       /* return address                / esp+16 */
        push    %ebp                   /* esp+12 */
        push    %edi                   /* esp+8 */
        push    %esi                   /* esp+4 */
        push    %ebx            /* esp */

/*       match        equ esi
 *       scan         equ edi
 *       chain_length equ ebp
 *       best_len     equ ebx
 *       limit        equ edx
 */

        mov     cur_match,%esi
        mov     _max_chain_length,%ebp /* chain_length = max_chain_length */
        mov     _strstart,%edi
        mov     %edi,%edx
        sub     $ MAX_DIST,%edx        /* limit = strstart-MAX_DIST */
        jae     limit_ok
        sub     %edx,%edx              /* limit = NIL */
limit_ok:
        add     $_slide+2,%edi         /* edi = offset(slide + strstart + 2) */
        mov     _prev_length,%ebx      /* best_len = prev_length */
        movw    -3(%ebx,%edi),%ax      /* ax = scan[best_len-1..best_len] */
        movw    -2(%edi),%cx           /* cx = scan[0..1] */
        cmp     _good_match,%ebx       /* do we have a good match already? */
        jb      do_scan
        shr     $2,%ebp                /* chain_length >>= 2 */
        jmp     do_scan

        .align  4
long_loop:
/* at this point, edi == scan+2, esi == cur_match */
        movw    -3(%ebx,%edi),%ax      /* ax = scan[best_len-1..best_len] */
        movw    -2(%edi),%cx           /* cx = scan[0..1] */
short_loop:
        dec     %ebp                   /* --chain_length */
        jz      the_end
/* at this point, di == scan+2, si == cur_match,
 * ax = scan[best_len-1..best_len] and cx = scan[0..1]
 */
        and     $ WSIZE-1, %esi
        movw    _prev(%esi,%esi),%si   /* cur_match = prev[cur_match] */
                                       /* top word of esi is still 0 */
        cmp     %edx,%esi       /* cur_match <= limit ? */
        jbe     the_end
do_scan:
        cmpw    _slide-1(%ebx,%esi),%ax/*check match at best_len-1 */
        jne     short_loop
        cmpw    _slide(%esi),%cx       /* check min_match_length match */
        jne     short_loop

        lea     _slide+2(%esi),%esi    /* si = match */
        mov     %edi,%eax              /* ax = scan+2 */
        mov     $ MAX_MATCH2,%ecx      /* scan for at most MAX_MATCH bytes */
#if defined(SYSV) || defined(__ELF__)
        repz;   cmpsw                  /* loop until mismatch */
#else
        repe;   cmpsw
#endif
        je      maxmatch               /* match of length MAX_MATCH? */
mismatch:
        movb    -2(%edi),%cl           /* mismatch on first or second byte? */
        subb    -2(%esi),%cl           /* cl = 0 if first bytes equal */
        xchg    %edi,%eax              /* edi = scan+2, eax = end of scan */
        sub     %edi,%eax              /* eax = len */
        sub     %eax,%esi              /* esi = cur_match + 2 + offset(slide) */
        sub     $_slide+2,%esi         /* esi = cur_match */
        subb    $1,%cl                 /* set carry if cl == 0 (cannot use DEC) */
        adc     $0,%eax                /* eax = carry ? len+1 : len */
        cmp     %ebx,%eax              /* len > best_len ? */
        jle     long_loop
        mov     %esi,_match_start      /* match_start = cur_match */
        mov     %eax,%ebx              /* ebx = best_len = len */
        cmp     $ MAX_MATCH,%eax       /* len >= MAX_MATCH ? */
        jl      long_loop
the_end:
        mov     %ebx,%eax              /* result = eax = best_len */
        pop     %ebx
        pop     %esi
        pop     %edi
        pop     %ebp
        ret
maxmatch:
        cmpsb
        jmp     mismatch


#if defined(USE_WIN32_ASSEMBLER)

#include "mpilib.h"

#if defined(SMITH)

extern unit reciph,recipl;
extern int  mshift;

#if !defined(_MSC_VER)
#error "This code needs a Microsoft compiler"
#endif

#if defined(_M_IX86)

#pragma warning(disable:4035)


unit P_QUO_DIGIT (unitptr dividend)
{
	__asm{
		mov	esi,DWORD PTR [dividend]
		push	ebp
		mov	eax, DWORD PTR [esi-8]
		not	eax
		mul	DWORD PTR [reciph]
		add	eax,DWORD PTR [reciph]
		adc	edx,0
		mov	ebx,eax
		mov	edi,edx
		mov	eax,DWORD PTR [esi-4]
		not	eax
		mul	DWORD PTR [recipl]
		inc	edx
		mov	ebp,edx
		and	ebp,edi
		and	ebp,1

		add	eax,ebx
		adc	edi,edx
		rcr	edi,1
		mov	eax,DWORD PTR [esi-4]
		not	eax
		mul	DWORD PTR [reciph]
		mov	ebx,eax
		mov	ecx,edx
		mov	eax, DWORD PTR [esi]
		not eax
		mul	DWORD PTR [recipl]
		xor	eax,ebx
		and	ebp,eax
		xor	eax,ebx
		add	eax,ebx
		adc	edx,ecx
		rcr	edx,1
		rcr	eax,1
		add	eax,edi
		adc	edx,0
		add	eax,ebp
		adc	edx,0
		shl	eax,1
		rcl	edx,1
		rcl eax,1
		rcl	edx,1
		rcl	eax,1
		and	eax,3
		mov	ecx,eax
		mov	ebx,edx
		mov	eax,DWORD PTR [esi]
		not eax
		mul	DWORD PTR [reciph]
		shl	eax,1
		rcl	edx,1
		add	eax,ebx
		adc	edx,ecx
		mov	ecx, DWORD PTR [mshift]
		cmp	DWORD PTR [mshift],32
		je	L2
		shrd	eax,edx,cl
		shr		edx,cl
		or	edx,edx
		je	L1
		mov	eax,-1
		jmp	L1
	L2:
		xchg	eax,edx
	L1:
		pop	ebp
	}
}

#pragma warning(default:4035)

#endif /* X86 */

#if defined(_M_PPC)
#error "We've not written the PowerPC Code Yet!"
#endif /* _M_PPC */

#if defined(_M_ALPHA)
#error "We've not written the Alpha Code Yet!"
#endif /* _M_ALPHA */

#if defined(_M_MRX000)
#error "We've not written the MIPS Code Yet!"
#endif /* _M_MRX000 */

#endif /*#defined(SMITH) */

#endif /* USE_WIN32_ASSEMBLER */

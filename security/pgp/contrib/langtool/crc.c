/************************************************************************/

/* CRC Routines. */
/*	These CRC functions are derived from code in chapter 19 of the book 
	"C Programmer's Guide to Serial Communications", by Joe Campbell.
	Generalized to any CRC width by Philip Zimmermann.
*/

#define byte unsigned char

#define CRCBITS 24	/* may be 16, 24, or 32 */
/* #define crcword unsigned short */	/* if CRCBITS is 16 */
#define crcword unsigned long		/* if CRCBITS is 24 or 32 */
/* #define maskcrc(crc) ((crcword)(crc)) */	/* if CRCBITS is 16 or 32 */
#define maskcrc(crc) ((crc) & 0xffffffL)	/* if CRCBITS is 24 */
#define CRCHIBIT ((crcword) (1L<<(CRCBITS-1))) /* 0x8000 if CRCBITS is 16 */
#define CRCSHIFTS (CRCBITS-8)

/*	Notes on making a good 24-bit CRC--
	The primitive irreducible polynomial of degree 23 over GF(2),
	040435651 (octal), comes from Appendix C of "Error Correcting Codes,
	2nd edition" by Peterson and Weldon, page 490.  This polynomial was
	chosen for its uniform density of ones and zeros, which has better
	error detection properties than polynomials with a minimal number of
	nonzero terms.  Multiplying this primitive degree-23 polynomial by
	the polynomial x+1 yields the additional property of detecting any
	odd number of bits in error, which means it adds parity.  This 
	approach was recommended by Neal Glover.

	To multiply the polynomial 040435651 by x+1, shift it left 1 bit and
	bitwise add (xor) the unshifted version back in.  Dropping the unused 
	upper bit (bit 24) produces a CRC-24 generator bitmask of 041446373 
	octal, or 0x864cfb hex.  

	You can detect spurious leading zeros or framing errors in the 
	message by initializing the CRC accumulator to some agreed-upon 
	nonzero "random-like" value, but this is a bit nonstandard.  
*/

#define CCITTCRC 0x1021 /* CCITT's 16-bit CRC generator polynomial */
#define PRZCRC 0x864cfbL /* PRZ's 24-bit CRC generator polynomial */
#define CRCINIT 0xB704CEL	/* Init value for CRC accumulator */

crcword crctable[256];		/* Table for speeding up CRC's */

/*	crchware simulates CRC hardware circuit.  Generates true CRC
	directly, without requiring extra NULL bytes to be appended 
	to the message.
	Returns new updated CRC accumulator.
*/
crcword crchware(byte ch, crcword poly, crcword accum)
{	int i;
	crcword data;
	data = ch;
	data <<= CRCSHIFTS;	/* shift data to line up with MSB of accum */
	i = 8;		/* counts 8 bits of data */
	do
	{	/* if MSB of (data XOR accum) is TRUE, shift and subtract poly */
		if ((data ^ accum) & CRCHIBIT)
			accum = (accum<<1) ^ poly;
		else
			accum <<= 1;
		data <<= 1;
	} while (--i);	/* counts 8 bits of data */
	return (maskcrc(accum));
}	/* crchware */


/*	mk_crctbl derives a CRC lookup table from the CRC polynomial.
	The table is used later by crcupdate function given below.
	mk_crctbl only needs to be called once at the dawn of time.
*/
void mk_crctbl(crcword poly)
{	int i;
	for (i=0; i<256; i++)
		crctable[i] = crchware((byte) i, poly, 0);
}	/* mk_crctbl */


/*	crcupdate calculates a CRC using the fast table-lookup method.
	Returns new updated CRC accumulator.
*/
crcword crcupdate(byte data, register crcword accum)
{	byte combined_value;

	/* XOR the MSByte of the accum with the data byte */
	combined_value = (accum >> CRCSHIFTS) ^ data;
	accum = (accum << 8) ^ crctable[combined_value];
	return (maskcrc(accum));
}	/* crcupdate */

/* Initialize the CRC table using our codes */
void init_crc()
{	mk_crctbl(PRZCRC);
}


/*
 * rsaglue2.c - These functions wrap and unwrap message digests (MDs) and
 * data encryption keys (DEKs) in padding and RSA-encrypt them into
 * multi-precision integers.  This layer of abstraction was introduced
 * to allow the transparent use of either the RSAREF Cryptographic
 * Toolkit from RSA Data Security Inc for the RSA calculations (where
 * the RSA patent applies), or, by using a different version of this
 * rsaglue module, we can use Philip Zimmermann's mpi library for the
 * RSA calculations.  The rsaglue.c module from PGP version 2.3a performs
 * the same functions as this module, but can be compiled to select the
 * use of mpilib functions instead of RSAREF as the underlying math engine.
 * That version of rsaglue.c would be suitable where the RSA patent does
 * not apply, such as Canada.
 *
 * This file uses RSAREF to perform the actual encryption and decryption.
 * It must be linked with the RSAREF library (rsaref.a, rsaref.lib,
 * or whatever it's called on your system) to function.
 *
 * This code only accepts PKCS-style padding.  Sorry, folks, but the
 * RSAREF routines won't do it any other way.  This will cause some
 * older messages and signatures trouble.
 * See pgformat.doc for a detailed description of the formats.
 *
 * (c) Copyright 1990-1996 by Philip Zimmermann.  All rights reserved.
 * The author assumes no liability for damages resulting from the use
 * of this software, even if the damage results from defects in this
 * software.  No warranty is expressed or implied.
 *
 * Note that while most PGP source modules bear Philip Zimmermann's
 * copyright notice, many of them have been revised or entirely written
 * by contributors who frequently failed to put their names in their
 * code.  Code that has been incorporated into PGP from other authors
 * was either originally published in the public domain or is used with
 * permission from the various authors.
 *
 * PGP is available for free to the public under certain restrictions.
 * See the PGP User's Guide (included in the release package) for
 * important information about licensing, patent restrictions on
 * certain algorithms, trademarks, copyrights, and export controls.
 */

#include <string.h> 	/* for mem*() */
#include <assert.h>
#include "mpilib.h"
#include "mpiio.h"
#include "pgp.h"
#include "rsaglue.h"
#include "random.h"	/* for cryptRandByte() */
#include "language.h"	/* for _LANG() */

char signon_legalese[] = _LANG("\
Uses the RSAREF(tm) Toolkit, which is copyright RSA Data Security, Inc.\n\
Distributed by the Massachusetts Institute of Technology.\n");

#include <global.h>
#include <rsaref.h>
#include <rsa.h>
/*
 * The functions we call in rsa.h are:
 *
 * int RSAPublicEncrypt PROTO_LIST
 *   ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
 *     R_RSA_PUBLIC_KEY *, R_RANDOM_STRUCT *));
 * int RSAPrivateEncrypt PROTO_LIST
 *   ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
 *     R_RSA_PRIVATE_KEY *));
 * int RSAPublicDecrypt PROTO_LIST
 *   ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
 *     R_RSA_PUBLIC_KEY *));
 * int RSAPrivateDecrypt PROTO_LIST
 *   ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
 *     R_RSA_PRIVATE_KEY *));
 */

/* Functions to convert to and from RSAREF's bignum formats */

void
rsaref2reg (unitptr to, byte *from, int frombytes)
/* Convert an RSAREF-style MSB-first array of bytes to an mpi-style
 * native-byte-order integer.  (global_precision units long.)
 */
{
	int tobytes;

	tobytes = units2bytes (global_precision);
	if (tobytes > frombytes) {
		memset(to, 0, tobytes - frombytes);
		memcpy((byte *)to + tobytes - frombytes, from, frombytes);
	} else {
		memcpy((byte *)to, from + frombytes - tobytes, tobytes);
	}
#ifndef HIGHFIRST
	hiloswap((byte *)to, tobytes);
#endif
} /* rsaref2reg */

void
reg2rsaref (byte *to, int tobytes, unitptr from)
/* Convert the other way, mpi format to an array of bytes. */
{
	int frombytes;

	frombytes = units2bytes(global_precision);

#ifdef HIGHFIRST
	if (tobytes > frombytes) {
		memset(to, 0, tobytes-frombytes);
		memcpy(to + tobytes - frombytes, (byte *)from, frombytes);
	} else {
		memcpy(to, (byte *)from + frombytes - tobytes, tobytes);
	}
#else
	if (tobytes > frombytes) {
		memcpy(to, (byte *)from, frombytes);
		memset(to + frombytes, 0, tobytes-frombytes);
	} else {
		memcpy(to, (byte *)from, tobytes);
	}
	hiloswap(to, tobytes);
#endif
} /* reg2rsaref */

int
make_RSA_PUBLIC_KEY(R_RSA_PUBLIC_KEY *rpk, unitptr e, unitptr n)
/* Given mpi's e and n, fill in an R_RSA_PUBLIC_KEY structure.
 * Returns -3 on error (key too big), 0 on success
 */
{
	rpk->bits = countbits(n);

	if (rpk->bits > MAX_RSA_MODULUS_BITS)
		return -3;

	reg2rsaref(rpk->modulus, MAX_RSA_MODULUS_LEN, n);
	reg2rsaref(rpk->exponent, MAX_RSA_MODULUS_LEN, e);
	return 0;
} /* make_RSA_PUBLIC_KEY */

/* Returns -1 on error, 0 on success */
int
make_RSA_PRIVATE_KEY(R_RSA_PRIVATE_KEY *rpk, unitptr e, unitptr d, unitptr p,
		     unitptr q, unitptr dp, unitptr dq, unitptr u, unitptr n)
/* Given a number of necessary mpi's, fill in an R_RSA_PRIVATE_KEY structure.
 * Returns -3 on error (key too big), 0 on success
 */
{
	rpk->bits = countbits(n);

	if (rpk->bits > MAX_RSA_MODULUS_BITS ||
	    countbits(p) > MAX_RSA_PRIME_BITS ||
	    countbits(q) > MAX_RSA_PRIME_BITS)
		return -3;

	reg2rsaref(rpk->modulus, MAX_RSA_MODULUS_LEN, n);
	reg2rsaref(rpk->publicExponent, MAX_RSA_MODULUS_LEN, e);
	reg2rsaref(rpk->exponent, MAX_RSA_MODULUS_LEN, d);
	/* The larger prime (p) first */
	reg2rsaref(rpk->prime[0], MAX_RSA_PRIME_LEN, q);
	reg2rsaref(rpk->prime[1], MAX_RSA_PRIME_LEN, p);
	/* d mod (p-1) and d mod (q-1) */
	reg2rsaref(rpk->primeExponent[0], MAX_RSA_PRIME_LEN, dq);
	reg2rsaref(rpk->primeExponent[1], MAX_RSA_PRIME_LEN, dp);
	/* 1/q mod p */
	reg2rsaref(rpk->coefficient, MAX_RSA_PRIME_LEN, u);
	return 0;
} /* make_RSA_PRIVATE_KEY */

/*
 * These functions hide all the internal details of RSA-encrypted
 * keys and digests.
 */

/* Abstract Syntax Notation One (ASN.1) Distinguished Encoding Rules (DER)
   encoding for RSA/MD5, used in PKCS-format signatures. */
static byte asn_array[] = {	/* PKCS 01 block type 01 data */
	0x30,0x20,0x30,0x0c,0x06,0x08,0x2a,0x86,0x48,0x86,0xf7,0x0d,
	0x02,0x05,0x05,0x00,0x04,0x10 };
/* This many bytes from the end, there's a zero byte */
#define ASN_ZERO_END 3

int
rsa_public_encrypt(unitptr outbuf, byteptr inbuf, short bytes,
	 unitptr E, unitptr N)
/* Encrypt a DEK with a public key.  Returns 0 on success.
 * <0 means there was an error.
 * -1: Generic error
 * -3: Key too big
 * -4: Key too small
 */
{
	unit temp[MAX_UNIT_PRECISION];
	unsigned int blocksize;
	int i;	/* Temporary, and holds error codes */
	R_RSA_PUBLIC_KEY PubKey;
	R_RANDOM_STRUCT Random;

	/* Fill in the R_RSA_PUBLIC_KEY structure as needed later. */
	i = make_RSA_PUBLIC_KEY(&PubKey, E, N);
	if (i < 0)
		return i;

	/* The RSAREF routines have their own random number generator
	 * to generate random padding.  The following code seeds it
	 * from PGP's random number generator.
	 */
	R_RandomInit(&Random);
	for (;;) {
		/* Bytes needed is an unsigned int */
		R_GetRandomBytesNeeded(&blocksize, &Random);
		if (!blocksize)
			break;
		if (blocksize > sizeof(temp))
			blocksize = sizeof(temp);
		for (i = 0; i < blocksize; i++)
			((byte *)temp)[i] = cryptRandByte();
		R_RandomUpdate(&Random, (byte *)temp, blocksize);

	}
	/* Pad and encrypt */
	i = RSAPublicEncrypt((byte *)temp, &blocksize,
			     inbuf, bytes, &PubKey, &Random);
	R_RandomFinal(&Random);	/* Clean up RSAREF's RNG */
	burn(Random);		/* Just to be sure */

	if (i)
		i = (i == RE_LEN) ? -4 : -1;

	rsaref2reg(outbuf, (byte *)temp, blocksize);

Cleanup:
	mp_burn(temp);
	burn(PubKey);
	return i < 0 ? i : 0;
} /* rsa_public_encrypt */

int
rsa_private_encrypt(unitptr outbuf, byteptr inbuf, short bytes,
	 unitptr E, unitptr D, unitptr P, unitptr Q, unitptr U, unitptr N)
/* Encrypt a message digest with a private key.
 * Returns <0 on error:
 * -1: generic error
 * -3: Key too big
 * -4: Key too small
 */
{
	unit temp[MAX_UNIT_PRECISION];
	unit DP[MAX_UNIT_PRECISION], DQ[MAX_UNIT_PRECISION];
	R_RSA_PRIVATE_KEY PrivKey;
	byte *p;
	int i;
	unsigned int blocksize;

	/* PGP doesn't store these coefficents, so we need to compute them. */
	mp_move(temp,P);
	mp_dec(temp);
	mp_mod(DP,D,temp);
	mp_move(temp,Q);
	mp_dec(temp);
	mp_mod(DQ,D,temp);

	p = (byte *)temp;

	i = make_RSA_PRIVATE_KEY(&PrivKey, E, D, P, Q, DP, DQ, U, N);
	if (i < 0)
		goto Cleanup;
	memcpy(p, asn_array, sizeof(asn_array)); /* ASN data */
	p += sizeof(asn_array);
	memcpy(p, inbuf, bytes);	/* User data */
	/* Pad and encrypt */
	i = RSAPrivateEncrypt((byte *)temp, &blocksize,
	                      (byte *)temp, bytes+sizeof(asn_array), &PrivKey);
	burn(PrivKey);
	if (i)
		i = (i == RE_LEN) ? -4 : -1;

	rsaref2reg(outbuf, (byte *)temp, blocksize);

Cleanup:
	burn(temp);

	return i;
} /* rsa_private_encrypt */

/* Remove a signature packet from an MPI */
/* Thus, we expect constant padding and the MIC ASN sequence */
int
rsa_public_decrypt(byteptr outbuf, unitptr inbuf,
	unitptr E, unitptr N)
/* Decrypt a message digest using a public key.  Returns the number of bytes
 * extracted, or <0 on error.
 * -1: Corrupted packet.
 * -3: Key too big
 * -4: Key too small
 * -5: Maybe malformed RSA packet
 * -7: Unknown conventional algorithm
 * -9: Malformed RSA packet
 */
{
	R_RSA_PUBLIC_KEY PubKey;
	unit temp[MAX_UNIT_PRECISION];
	unsigned int blocksize;
	int i;
	byte *front, *back;

	i = make_RSA_PUBLIC_KEY(&PubKey, E, N);
	if (i < 0)
		return i;
	blocksize = countbytes(inbuf);
	reg2rsaref((byte *)temp, blocksize, inbuf);

	i = RSAPublicDecrypt((byte *)temp, &blocksize,
			     (byte *)temp, blocksize, &PubKey);
	burn(PubKey);
	if (i) {
		mp_burn(temp);
		if (i == RE_LEN)
			return -4;
		if (i == RE_DATA)
			return -5;
		return -1;
	}
	front = (byte *)temp;
	back = front+blocksize;

	if (memcmp(front, asn_array, sizeof(asn_array))) {
		mp_burn(temp);
		return -7;
	}
	front += sizeof(asn_array);

/* We're done - copy user data to outbuf */
	if (back < front)
		goto ErrorReturn;
	blocksize = back-front;
	memcpy(outbuf, front, blocksize);
	mp_burn(temp);
	return blocksize;
ErrorReturn:
	mp_burn(temp);
	return -9;
} /* rsa_public_decrypt */

/* We expect to find random padding and an encryption key */
int
rsa_private_decrypt(byteptr outbuf, unitptr inbuf,
	 unitptr E, unitptr D, unitptr P, unitptr Q, unitptr U, unitptr N)
/* Decrypt an encryption key using a private key.  Returns the number of bytes
 * extracted, or <0 on error.
 * -1: Generic error
 * -3: Key too big
 * -4: Key too small
 * -5: Maybe malformed RSA
 * -7: Unknown conventional algorithm
 * -9: Malformed RSA packet
 */
{
	R_RSA_PRIVATE_KEY PrivKey;
	byte *front;
	unsigned int blocksize;
	unit temp[MAX_UNIT_PRECISION];
	unit DP[MAX_UNIT_PRECISION], DQ[MAX_UNIT_PRECISION];
	int i;

	/* PGP doesn't store (d mod p-1) and (d mod q-1), so compute 'em */
	mp_move(temp,P);
	mp_dec(temp);
	mp_mod(DP,D,temp);
	mp_move(temp,Q);
	mp_dec(temp);
	mp_mod(DQ,D,temp);

	i = make_RSA_PRIVATE_KEY(&PrivKey, E, D, P, Q, DP, DQ, U, N);
	mp_burn(DP);
	mp_burn(DQ);
	mp_burn(temp);
	
	if (i < 0)
		return i;

	blocksize = countbytes(inbuf);
	reg2rsaref((byte *)temp, blocksize, inbuf);
	i = RSAPrivateDecrypt((byte *)temp, &blocksize,
			      (byte *)temp, blocksize, &PrivKey);
	burn(PrivKey);
	if (i) {
		if (i == RE_LEN)
			return -4;
		if (i == RE_DATA)
			return -5;
		return -1;
	}
	front = (byte *)temp;			/* Start of block */

	memcpy(outbuf, front, blocksize);
	mp_burn(temp);
	return blocksize;

Corrupted:
	mp_burn(temp);
	return -9;
} /* rsa_private_decrypt */

/*
 * Stub to replace RSAREF's NN_ModExp with a call to the mpilib's
 * faster mp_modexp.  (A bit over 3x faster on an IBM PC.)
 * It's too bad that RSAREF's NN routines are pretty clean, while the
 * mpilib is a hard-to-follow kludge.  But the mpilib is pretty fast,
 * especially on 16-bit machines, so the kludginess is forgivable.
 * Note that we are still using RSAREF, but we are just using a faster
 * modulo exponentiation routine.
 * If you comment out the following block of code, you get a (much slower)
 * pure RSAREF version.
 */
#ifdef USEMPILIB

/*
 * The mpilib keeps numbers in native byte order, in arrays global_precision
 * "units" long.  RSAREF keeps numbers in little-endian arrays of 32-bit
 * "digits".
 */
static void
nn2mpi(unit *mpi, word32 *nn, unsigned nndigits)
{
	/* nndigits must be <= global_precision */
	unsigned i;
	word32 *p;

	assert((units2bytes(global_precision) & 3) == 0);
	i = units2bytes(global_precision) >> 2;
	if (nndigits > i)
		nndigits = i;
	i -= nndigits;

#ifdef HIGHFIRST
	p = (word32 *)(mpi+global_precision);
	while (nndigits--)
		*--p = *nn++;
	while (i--)
		*--p = 0;
#else
	p = (word32 *)mpi;
	while (nndigits--)
		*p++ = *nn++;
	while (i--)
		*p++ = 0;
#endif
}

static void
mpi2nn(word32 *nn, unsigned nndigits, unit *mpi)
{
	/* nndigits must be >= global_precision */
	unsigned i;
	word32 *p;

	assert((units2bytes(global_precision) & 3) == 0);
	i = units2bytes(global_precision) >> 2;
	if (i > nndigits)
		i = nndigits;
	nndigits -= i;

#ifdef HIGHFIRST
	p = (word32 *)(mpi+global_precision);
	while (i--)
		*nn++ = *--p;
#else
	p = (word32 *)mpi;
	while (i--)
		*nn++ = *p++;
#endif
	while (nndigits--)
		*nn++ = 0;
}

void
NN_ModExp(word32 *result, word32 *base, word32 *exponent, unsigned expdigits,
	  word32 *modulus, unsigned moddigits)
{
	unit a[MAX_UNIT_PRECISION], b[MAX_UNIT_PRECISION];
	unit c[MAX_UNIT_PRECISION], d[MAX_UNIT_PRECISION];
	int i;
	unsigned oldprecision;

	oldprecision = global_precision;
	set_precision(MAX_UNIT_PRECISION);

	nn2mpi(b, base, moddigits);
	nn2mpi(c, exponent, expdigits);
	nn2mpi(d, modulus, moddigits);

	i = mp_modexp(a, b, c, d);
	assert(i == 0);
	mpi2nn(result, moddigits, a);
	set_precision(oldprecision);
}

#endif

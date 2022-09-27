/******************************************************************************

20030618 Seanni
	lcrng_fill()

20030528 Seanni (Sze-Yao Ni)
	Initial version created,
	Modified from random.c of LCRNG.zip, source code of book 
	"Applied Cryprography, 2nd"

*******************************************************************************/
#include "lcrng.h"

/* The following is the original comment of the source code random.c */
/*
 $Id: random.c,v $

 This program is public domain and was written by William S. England
 (Oct 1988).  It is based on an article by:

 Stephen K. Park and Keith W. Miller. RANDOM NUMBER GENERATORS:
 GOOD ONES ARE HARD TO FIND. Communications of the ACM,
 New York, NY.,October 1988 p.1192

 Modifications;

 $Log: random.c,v $

######

 The following is a portable c program for generating random numbers.
 The modulus and multiplier have been extensively tested and should
 not be changed except by someone who is a professional Lehmer generator
 writer.  THIS GENERATOR REPRESENTS THE MINIMUM STANDARD AGAINST WHICH
 OTHER GENERATORS SHOULD BE JUDGED. ("Quote from the referenced article's
 authors. WSE" )
*/

/*
**  These are pre-calculated  below to compensate for c 
**  compilers that may overflow when building the code.
**
**  q = (m / a)
**  r = (m % a)
*/

#define LCRNG_PROTECT(v) ((v) ^ 0x3F992E4D)

static unsigned long lcrng_m  = LCRNG_PROTECT((unsigned long)2147483647);
static unsigned long lcrng_q  = LCRNG_PROTECT((unsigned long)44488);

static unsigned int lcrng_a = LCRNG_PROTECT(48271);
static unsigned int lcrng_r = LCRNG_PROTECT(3399);

/*
** F(z)	= (az)%m
**	= az-m(az/m)
**
** F(z)  = G(z)+mT(z)
** G(z)  = a(z%q)- r(z/q)
** T(z)  = (z/q) - (az/m)
**
** F(z)  = a(z%q)- rz/q+ m((z/q) - a(z/m))
** 	 = a(z%q)- rz/q+ m(z/q) - az
*/

unsigned long lcrng_rand(unsigned long *prng)
{
	register int lo, hi, test;

    hi   = *prng / LCRNG_PROTECT(lcrng_q);
    lo   = *prng % LCRNG_PROTECT(lcrng_q);

    test = LCRNG_PROTECT(lcrng_a)*lo - LCRNG_PROTECT(lcrng_r)*hi;

    if (test > 0)
		*prng = test;
    else
		*prng = test+ LCRNG_PROTECT(lcrng_m);
	return *prng;
}

void lcrng_fill(unsigned long *prng, void *fillat, unsigned len)
{
	int i, n=len / sizeof(unsigned long);
	for (i=0; i<n; i++)
	{
		((unsigned long *)fillat)[i]=lcrng_rand(prng);
	}
	fillat = (unsigned long *)fillat + n;
	len = len % sizeof(unsigned long);
	while(len)
	{
		*(unsigned char *)fillat=(unsigned char)lcrng_rand(prng);
		fillat = (unsigned char *)fillat + 1;
		len--;
	}
}


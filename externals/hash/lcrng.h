#ifndef __LCRNG_H
#define __LCRNG_H

#ifdef __cplusplus
extern "C" {
#endif

static __inline void lcrng_srand(unsigned long *prng, unsigned long seed)
{
	*prng=seed;
}

extern unsigned long lcrng_rand(unsigned long *prng);

extern void lcrng_fill(unsigned long *prng, void *fillat, unsigned len);

#ifdef __cplusplus
}
#endif

#endif


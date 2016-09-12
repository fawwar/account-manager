#ifndef __SHA1_H_
#define __SHA1_H_

#ifdef __GNUC__
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SHA_UI32
#define SHA_UI32 unsigned int
#endif

#define SHA_BLOCKBYTES 64
#define SHA_BLOCKWORDS 16

#define SHA_HASHBYTES 20
#define SHA_HASHWORDS 5

/* SHA context. */
typedef struct _SHAContext 
{
	unsigned int key[SHA_BLOCKWORDS];
	SHA_UI32 iv[SHA_HASHWORDS];
#ifdef __GNUC__
	u_int64_t bytes;
#else
	SHA_UI32 bytesHi, bytesLo;
#endif
} SHAContext;

extern void   SHA1Init(SHAContext *);
extern void   SHA1Update(SHAContext *, const unsigned char *buf, unsigned int len);
extern void   SHA1Final(SHAContext *, unsigned char digest[SHA_HASHBYTES]);

#ifdef __cplusplus
}
#endif

#endif

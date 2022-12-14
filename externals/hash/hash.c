#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/time.h>   // #include<time.h>
#endif // _WIN32

#include <time.h>
#include "hash.h"
#include "lcrng.h"
#include "md5.h"

#ifdef _WIN32
	#include <time.h>
	#include <windows.h>

	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif

	struct timezone
	{
		int  tz_minuteswest; /* minutes W of Greenwich */
		int  tz_dsttime;     /* type of dst correction */
	};

	int gettimeofday(struct timeval *tv, struct timezone *tz)
	{
		FILETIME ft;
		unsigned __int64 tmpres = 0;
		static int tzflag = 0;

		if (NULL != tv)
		{
			GetSystemTimeAsFileTime(&ft);

			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;

			tmpres /= 10;  /*convert into microseconds*/
						   /*converting file time to unix epoch*/
			tmpres -= DELTA_EPOCH_IN_MICROSECS;
			tv->tv_sec = (long)(tmpres / 1000000UL);
			tv->tv_usec = (long)(tmpres % 1000000UL);
		}

		if (NULL != tz)
		{
			if (!tzflag)
			{
				_tzset();
				tzflag++;
			}
			tz->tz_minuteswest = _timezone / 60;
			tz->tz_dsttime = _daylight;
		}

		return 0;
	}
#endif // _WIN32

unsigned long Salt()
{	
	typedef unsigned int u32;

	struct timeval time; 
	gettimeofday(&time,NULL);
	srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

	u32 ulsalt = ( (u32)(rand()) << 16 ) |    // bit[31:18]
		( (u32)(rand()) << 2 ) |              // bit[17:3]
		( (u32)(rand())& 0x3UL ) ;            // bit[2:0]

	return ulsalt;
}

void EncodeMD5(unsigned long salt, const unsigned char* text, int text_size, unsigned char out_md5_text[MD5LEN])
{
	unsigned long rand_value[8];	// 8 * 4btye = 32byte
	
	//printf(" Rand Value = ");
	/* create 32 byte rand value */
	lcrng_srand(rand_value, salt);
	int i;
	for(i = 0; i < 8; ++i){

		rand_value[i] = lcrng_rand(rand_value);
		//printf("%lx ",rand_value[i]);
	}
	
	//printf(" \n");

	/* create md5 text */
	md5_state_t state;
	md5_init(&state);
	md5_append(&state, (md5_byte_t *)text, text_size);
	md5_append(&state, (md5_byte_t *)rand_value, 32);
	md5_finish(&state, out_md5_text);
}

void TextEncode(const unsigned char* text, int text_size, char* out_hash_text)
{
	unsigned long ulsalt = Salt();//0xfe1bac03;
	
	//printf("Salt = %lx\n", ulsalt);

	unsigned char md5_text[MD5LEN];
	EncodeMD5(ulsalt, text, text_size, md5_text);

	//printf(" MD5 = ");
	//int j;
	//for(j = 0; j < MD5LEN; ++j){
    //
	//	printf("%x ",md5_text[j]);
	//}
	//printf(" \n");

	
	unsigned char uc_hash_text[HASHLEN];
	unsigned char *pc_hash_text = &uc_hash_text[0];
	memcpy(pc_hash_text, md5_text, MD5LEN);

	unsigned char salt[SALTLEN];

	salt[0] = ulsalt >> 24;
	salt[1] = ulsalt >> 16;
	salt[2] = ulsalt >> 8;
	salt[3] = ulsalt;

	//printf("salt[0] = %x , salt[1] = %x, salt[2] = %x , salt[3] = %x \n",salt[0],salt[1],salt[2],salt[3]);

	memcpy(pc_hash_text + MD5LEN, salt, SALTLEN);

	printf("<info> Hash Code = ");
	int k;
	for(k = 0; k < HASHLEN; ++k){

		printf("%x ",uc_hash_text[k]);
		sprintf(out_hash_text+(k*2), "%02x", uc_hash_text[k]);
	}

	printf(" \n");
	
}

void TextMD5(unsigned long salt, const unsigned char* text, int text_size, char* out_md5_text)
{
	unsigned char uc_md5_text[MD5LEN];
	EncodeMD5(salt, text, text_size, uc_md5_text);	

	printf("<info> MD5 = ");
	int i;
	for(i = 0; i < MD5LEN; ++i){
		printf("%x ",uc_md5_text[i]);
		sprintf(out_md5_text + (i*2), "%02x", uc_md5_text[i]);
	}
	printf(" \n");
}
#ifndef HASH_HPP_
#define HASH_HPP_

#ifdef __cplusplus
extern "C" 
{
#endif

#define SALTLEN 4   // 4btye
#define MD5LEN 16   // 16 byte
#define HASHLEN 20  // 20byte

/*
 * create 4byte salt to rand 32 byte value,
 * use source text and 32 byte rand value to hash md5 text
 * we combine 16 byte md5 text and 4 byte salt to our password
 * ex: "admin" + 32byte value (hash md5) --> 16byte md5 text
 *      16byte md5 text + 4byte salt = 20byte password
 */

void TextEncode(const unsigned char* text, int text_size, char* out_hash_text);

void TextMD5(unsigned long salt, const unsigned char* text, int text_size, char* out_md5_text);


#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif


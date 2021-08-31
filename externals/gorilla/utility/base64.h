#ifndef __BASE64_H
#define __BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

// Estimate output buffer size
#define Base64Length(len) (((len) + 3 - (len) % 3) * 4 / 3 + 1)
#define Base64DecodeLength(len) ((len)*3 / 4)
//
// Input:
//        in, len: input buffer and length
// Output:
//        out, olen: output buffer and length
//
// If the length of output buffer is not enough, the output is incompleted.
extern int Base64Encode(
    const unsigned char* in, unsigned len, char* out, unsigned olen);

// olen return length (null terminating char included)
// return allocated buffer
extern char* Base64EncodeLines(
    const unsigned char* in, unsigned len, unsigned* olen);

//
// Input:
//        in: null-terminated base64 string
// Output:
//        out: pointer to return buffer, the buffer must be large enough to hold
//        decoded data
//        return length of returned data
//        return -1 on error (out_max is not enough)
//
extern int Base64Decode(const char* in, unsigned char* out, int out_max);

// Immune to prefixing and postfixing white space of each input line, but
// input string may be altered temporarily
extern int Base64DecodeLines(char* in, unsigned char* out, int out_max);

// 0: not a valid base64 char
// 1: a base64 char
extern int IsBase64Char(unsigned char ch);

// return the number of valid Base64 char
extern int Base64GetValidStringLen(const char* start);

#ifdef __cplusplus
}
#endif

#endif

#include <ctype.h>
#include <stdlib.h>

#include "base64.h"

// must be a multiple of 3
#define BIN_PER_LINE 48

static int base64_val[128] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1,
    -1 };

static char base64_char[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
    '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

int Base64Encode(
    const unsigned char* in, unsigned len, char* out, unsigned olen)
{
    int ret = 0;
    // takes 3 bytes
    while (len >= 3 && olen > 4 + 4) {
        *out++ = base64_char[in[0] >> 2];
        *out++ = base64_char[((in[0] << 4) & 0x30) | (in[1] >> 4)];
        *out++ = base64_char[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
        *out++ = base64_char[in[2] & 0x3f];
        olen -= 4;
        len -= 3;
        in += 3;
        ret += 4;
    }

    // 1-2 remaining bytes
    if (len > 0 && olen > 4) {
        unsigned char frag;
        *out++ = base64_char[in[0] >> 2];
        frag = (unsigned char)((in[0] << 4) & 0x30);
        if (len > 1)
            frag |= in[1] >> 4;
        *out++ = base64_char[frag];
        *out++ = (char)((len < 2) ? '=' : base64_char[(in[1] << 2) & 0x3c]);
        *out++ = '=';
        ret += 4;
    }
    *out = '\0';
    return ret;
}

int Base64Decode(const char* in, unsigned char* out, int out_max)
{
    int len = 0;
    register unsigned char digit1, digit2, digit3, digit4;

    do {
        digit1 = in[0];
        if (digit1 > 127 || base64_val[digit1] == -1)
            return -1;
        digit2 = in[1];
        if (digit2 > 127 || base64_val[digit2] == -1)
            return -1;
        digit3 = in[2];
        if (digit3 > 127 || ((digit3 != '=') && (base64_val[digit3] == -1)))
            return -1;
        digit4 = in[3];
        if (digit4 > 127 || ((digit4 != '=') && (base64_val[digit4] == -1)))
            return -1;
        in += 4;

        if (len == out_max)
            return -1;
        /* digits are already sanity-checked */
        *out++ = (char)((base64_val[digit1] << 2) | (base64_val[digit2] >> 4));
        len++;
        if (digit3 != '=') {
            if (len == out_max)
                return -1;
            *out++ = (char)(((base64_val[digit2] << 4) & 0xf0)
                | (base64_val[digit3] >> 2));
            len++;
            if (digit4 != '=') {
                if (len == out_max)
                    return -1;
                *out++ = (char)(((base64_val[digit3] << 6) & 0xc0)
                    | base64_val[digit4]);
                len++;
            }
        }
    } while (*in && digit4 != '=');

    return len;
}

int IsBase64Char(unsigned char ch)
{
    if (ch > 127 || base64_val[ch] == -1)
        return 0;
    return 1;
}

int Base64GetValidStringLen(const char* start)
{
    int count = 0;
    while (*start) {
        if (*(unsigned char*)start > 127
            || base64_val[*(unsigned char*)start] == -1) {
            while (*start == '=') {
                count++;
                start++;
            }
            return count;
        }
        count++;
        start++;
    }
    return count;
}

// olen_ret return length (null terminating char included)
char* Base64EncodeLines(
    const unsigned char* in, unsigned len, unsigned* olen_ret)
{
    unsigned int omax
        = ((len / BIN_PER_LINE) + 2) * (Base64Length(BIN_PER_LINE) + 2) + 1;
    char* out;
    unsigned int at = 0;
    int ret;
    int part;

    if (olen_ret != NULL)
        *olen_ret = 0;

    out = (char*)malloc(omax);
    if (out == NULL)
        return 0;

    while (len) {
        part = len;
        if (part > BIN_PER_LINE)
            part = BIN_PER_LINE;
        ret = Base64Encode(in, part, out + at, omax - at);
        in += part;
        len -= part;
        at += ret;
        out[at] = '\n';
        at++;
    }

    out[at++] = 0;

    if (olen_ret != NULL)
        *olen_ret = at;
    return out;
}

// input buffer may be changed temporarily
int Base64DecodeLines(char* in, unsigned char* out, int out_max)
{
    int l;
    int ret;
    int olen = 0;
    char keep;

    while (*in != '\0') {
        l = Base64GetValidStringLen(in);
        if (l == 0) {
            if (isspace((unsigned char)*in)) {
                in++; // skip white space
                continue;
            }
            return -1; // unknown character encountered
        }

        keep = in[l];
        in[l] = 0; // terminate it
        ret = Base64Decode(in, out, out_max);
        in[l] = keep;

        if (ret == -1)
            return -1;
        out += ret;
        olen += ret;
        out_max -= ret;

        if (keep == '\0')
            break;
        in += l + 1;
    }
    return olen;
}

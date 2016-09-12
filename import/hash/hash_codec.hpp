#ifndef HASH_CODEC_HPP_
#define HASH_CODEC_HPP_

#include <string>
#include <string.h>
#include "hash.h"
#include "base64.h"

namespace gorilla
{
	namespace hash
	{
		inline std::string TextEncodeHash(const std::string& str_text)
		{
			std::string hash_string;
			try{
				const size_t TEXT_LEN = str_text.size();
				unsigned char* uc_text = new unsigned char[TEXT_LEN];
				memcpy(uc_text, str_text.c_str(), TEXT_LEN);

				char hash_text[HASHLEN * 2 + 1] = {00};
				TextEncode(uc_text, TEXT_LEN, hash_text);
				delete []uc_text;

				hash_string = hash_text;
			}
			catch(...){
				return hash_string;
			}

			return hash_string;
		}

		inline std::string DecodeBase64(const std::string& str_text)
		{
			std::string decode_text;
			try{
				int text_size = str_text.size();
				unsigned char* pDecodeBuffer = new unsigned char[text_size];
				int base64_decode_size = Base64Decode(str_text.c_str(), pDecodeBuffer, text_size);
				decode_text.assign(pDecodeBuffer, pDecodeBuffer + base64_decode_size);
				delete []pDecodeBuffer;
			}
			catch(...){
				return decode_text;
			}

			return decode_text;
		}
	}
}

#endif
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <sstream>
#include "HttpUtil.h"
#include "mime.h"

namespace gorilla
{
namespace http
{

static unsigned char to_hex(unsigned char x)
{
    return x + (x > 9 ? ('A'-10) : '0');
}

static unsigned char from_hex(unsigned char ch)
{
    if (ch <= '9' && ch >= '0')
    {
        ch -= '0';
    }
    else if (ch <= 'f' && ch >= 'a')
    {
        ch -= 'a' - 10;
    }
    else if (ch <= 'F' && ch >= 'A')
    {
        ch -= 'A' - 10;
    }
    else
    {
        ch = 0;
    }
    return ch;
}

std::string HttpUtil::urlDecode(const std::string& s)
{
    std::string result;
    std::string::size_type i;
    for (i = 0; i < s.size(); ++i)
    {
        if (s[i] == '+')
        {
            result += ' ';
        }
        else if (s[i] == '%' && s.size() > i+2)
        {
            const unsigned char ch1 = from_hex(s[i+1]);
            const unsigned char ch2 = from_hex(s[i+2]);
            const unsigned char ch = (ch1 << 4) | ch2;
            result += ch;
            i += 2;
        }
        else
        {
            result += s[i];
        }
    }
    return result;
}

std::string HttpUtil::urlEncode(const std::string& s)
{
    std::ostringstream os;
    for (std::string::const_iterator ci = s.begin(); ci != s.end(); ++ci)
    {
        if((*ci >= 'a' && *ci <= 'z') ||
             (*ci >= 'A' && *ci <= 'Z') ||
             (*ci >= '0' && *ci <= '9') )
        {
            os << *ci;            
        }
        else if(*ci == ' ')
        {
            os << '+';            
        }
        else
        {
            os << '%' << to_hex(*ci >> 4) << to_hex(*ci % 16);
        }        
    }
    return os.str();    
}

std::pair<std::string, std::string> HttpUtil::split2(const std::string& s, char c)
{
    std::size_t pos = s.find_first_of(c);
    if(pos == std::string::npos)
    {
        return std::pair<std::string, std::string>(boost::algorithm::trim_copy(s), "");
    }
    else
    {
        return std::pair<std::string, std::string>(
            boost::algorithm::trim_copy(std::string(s.begin(), s.begin() + pos)),
            boost::algorithm::trim_copy(std::string(s.begin() + pos + 1, s.end() ))
        );
    }
}

std::map<std::string, std::string> HttpUtil::splitn(const std::string& s)
{
    std::map<std::string, std::string> result;
    auto v2 = split2(s, ';');
    std::string value = v2.second;
    std::vector<std::string> vs;
    boost::split(vs, value,boost::is_any_of(";"));
    for(auto& it: vs)
    {
      auto kv = split2(it, '=');
      if(kv.second.size() >= 2)
      {
        if(kv.second.front() == '"' &&
           kv.second.back() == '"')
        {
          kv.second.erase(0,1);
          kv.second.erase(kv.second.size() -1);
        }
      }
      if(!kv.first.empty())
      {
        result[kv.first] = kv.second;
      }
    }
    return result;
}

std::string HttpUtil::gzipCompress(const std::string& data)
{
    namespace bio = boost::iostreams;
    std::stringstream compressed;
    std::stringstream origin(data);
    bio::filtering_streambuf<bio::input> out;
    out.push(bio::gzip_compressor(bio::gzip_params()));
    out.push(origin);
    bio::copy(out, compressed);
    return compressed.str();
}

std::string HttpUtil::gzipDecompress(const std::string& data)
{
    namespace bio = boost::iostreams;
    std::stringstream compressed(data);
    std::stringstream decompressed;
    bio::filtering_streambuf<bio::input> out;
    out.push(bio::gzip_decompressor());
    out.push(compressed);
    bio::copy(out, decompressed);
    return decompressed.str();
}


std::string HttpUtil::zipCompress(const std::string& data)
{
    namespace bio = boost::iostreams;
    std::stringstream compressed;
    std::stringstream origin(data);
    bio::filtering_streambuf<bio::input> out;
    out.push(bio::zlib_compressor(bio::zlib_params(bio::zlib::best_compression)));
    out.push(origin);
    bio::copy(out, compressed);
    return compressed.str();
}

std::string HttpUtil::zipDecompress(const std::string& data)
{
    namespace bio = boost::iostreams;
    std::stringstream compressed(data);
    std::stringstream decompressed;
    bio::filtering_streambuf<bio::input> out;
    out.push(bio::zlib_decompressor());
    out.push(compressed);
    bio::copy(out, decompressed);
    return decompressed.str();
}

static const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string HttpUtil::base64Encode(const std::string& data)
{
  const char* bytes_to_encode = data.data();
  int in_len = data.length();
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];
  while (in_len--)
  {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) 
    {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;
      for(i = 0; i < 4; i++) 
      {
        ret += base64_chars[char_array_4[i]];
      }
      i = 0;
    }
  }
  if (i)
  {
    for(j = i; j < 3; j++)
    {
      char_array_3[j] = '\0';
    }
    char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    for (j = 0; (j < i + 1); j++)
    {
      ret += base64_chars[char_array_4[j]];
    }
    while((i++ < 3))
    {
      ret += '=';
    }
  }
  return ret;  
}

std::string HttpUtil::base64Decode(const std::string& encoded_string)
{
    //http://www.adp-gmbh.ch/cpp/common/base64.html
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i < 4; i++) char_array_4[i] = base64_chars.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; i < 3; i++) ret += char_array_3[i];
            i = 0;
        }
    }
    if (i) {
        for (j = i; j <4; j++) char_array_4[j] = 0;
        for (j = 0; j <4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    return ret;
}

std::string HttpUtil::getContentType(const std::string& fileName)
{
    int i = 0;
    while(mimeList[i][0])
    {
       if(boost::algorithm::iends_with(fileName, std::string(".") + mimeList[i][0]))
       {
           return mimeList[i][1];
       }
       i++;
    }
    return "application/octet-stream";
}

}
}
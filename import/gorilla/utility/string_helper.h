#pragma once

#include <stdarg.h>
#include <string>

namespace boost
{
namespace uuids
{
    struct uuid;
}
}

namespace gorilla
{
namespace utility
{

    //
    // String conversion functions
    //
    extern bool StringToInteger(const char* szValue, int& v);
    extern bool StringToInteger(
        const char* szValue, int& v, int nMin, int nMax);
    extern bool StringToByte(const char* szValue, unsigned char& b);
    // convert exactly b byte from hex string with no separator, return FALSE if
    // string is invalid or not long enough
    extern bool HexStringToBytes(
        const char* str, unsigned char* ptr, int nByte);
    extern bool HexStringToUnsignedInteger(
        const char* str, int nStrLen, unsigned* value);
    extern void BytesToHexString(
        const unsigned char* ptr, unsigned b, char* szText);

    extern bool StringToGUID(const char* szText, boost::uuids::uuid& guid);
    extern void GUIDToString(const boost::uuids::uuid& guid, std::string& str);


} // utility

} // gorilla

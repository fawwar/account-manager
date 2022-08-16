/******************************************************************************

20141022 Seanni
   Use boost to replace implementation


Description:

    Need boost/config.hpp (StringHelper.h) for BOOST_WINDOWS

*******************************************************************************/

#include "gorilla/utility/string_helper.h"
#include <boost/config.hpp>
#pragma warning(disable : 4996)
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <vector>
#include <stdio.h>

namespace gorilla
{
namespace utility
{

    /////////////////////////////////////////////////////////////////////////////
    //
    // String conversion functions
    //
    /////////////////////////////////////////////////////////////////////////////
    bool StringToInteger(const char* szValue, int& v)
    {
        char* pEnd = NULL;
        long lValue = strtol(szValue, &pEnd, 0);
        if (pEnd == szValue)
            return false;
        v = lValue;
        return true;
    }

    // if false returned, v is not changed
    bool StringToInteger(const char* szValue, int& v, int nMin, int nMax)
    {
        char* pEnd = NULL;
        long lValue = strtol(szValue, &pEnd, 0);
        if (pEnd == szValue)
            return false;
        if (lValue < nMin || lValue > nMax)
            return false;
        v = lValue;
        return true;
    }

    // if false is returned, v is not changed
    bool StringToByte(const char* szValue, unsigned char& v)
    {
        char* pEnd = NULL;
        long lValue = strtol(szValue, &pEnd, 0);
        if (pEnd == szValue)
            return false;
        if (lValue < 0 || lValue > 255)
            return false;
        v = (unsigned char) lValue;
        return true;
    }

    // convert exactly nByte byte, no separator
    bool HexStringToBytes(const char* str, unsigned char* ptr, int nByte)
    {
        char hex[3];
        int c = 0;
        hex[2] = 0;
        if (nByte == 0)
            return false;
        while (isxdigit(*str)) {
            hex[c++] = *(str++);
            if (c == 2) {
                *(ptr++) = (unsigned char) strtoul(hex, NULL, 16);
                nByte--;
                if (nByte == 0)
                    return true;
                c = 0;
            }
        }
        return false;
    }

    // convert exactly nStrLen hex characters to one unsigned value if nStrLen
    // != 0, if nStrLen==0, at most 8 characters will be converted
    bool HexStringToUnsignedInteger(
        const char* str, int nStrLen, unsigned* value)
    {
        char hex[9];
        int c = 0;
        while (isxdigit(*str)) {
            hex[c++] = *(str++);
            if (c == 8 || (nStrLen != 0 && c == nStrLen)) {
                hex[c] = 0;
                *value = strtoul(hex, NULL, 16);
                return true;
            }
        }
        if (c == 0 || nStrLen != 0)
            return false; // c<nStrLen
        hex[c] = 0;
        *value = strtoul(hex, NULL, 16);
        return true;
    }

    void BytesToHexString(const unsigned char* ptr, unsigned b, char* szText)
    {
        static const char* hexchar = "0123456789ABCDEF";
        while (b--) {
            *(szText++) = hexchar[(*ptr >> 4) & 0x0F];
            *(szText++) = hexchar[*(ptr++) & 0x0F];
        }
        *szText = 0;
    }

    bool StringToGUID(const char* str, boost::uuids::uuid& guid)
    {
        boost::uuids::string_generator gen;
        try {
            guid = gen(str);
        } catch (std::exception&) {
            return false;
        }
        return true;
    }

    void GUIDToString(const boost::uuids::uuid& guid, std::string& str)
    {
        str = to_string(guid);
        //    swprintf_s(sz, size,
        //    L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        //       guid.Data1, (unsigned)guid.Data2, (unsigned)guid.Data3,
        //       (unsigned)guid.Data4[0], (unsigned)guid.Data4[1],
        //       (unsigned)guid.Data4[2], (unsigned)guid.Data4[3],
        //       (unsigned)guid.Data4[4], (unsigned)guid.Data4[5],
        //       (unsigned)guid.Data4[6], (unsigned)guid.Data4[7]);
    }


} // utility

} // gorilla

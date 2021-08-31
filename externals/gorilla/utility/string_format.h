#pragma once

#include <stdarg.h>
#include <string>

#include <boost/config.hpp>

namespace gorilla
{
namespace utility
{
    // return dst
    extern std::string& string_append_v(
        std::string& dst, const char* format, va_list ap);
    // return dst
    extern std::wstring& string_append_v(
        std::wstring& dst, const wchar_t* format, va_list ap);

    inline std::string& string_append(std::string& str, const char* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        string_append_v(str, format, ap);
        va_end(ap);
        return str;
    }

    inline std::wstring string_append(
        std::wstring& str, const wchar_t* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        string_append_v(str, format, ap);
        va_end(ap);
        return str;
    }

    inline std::string string_format_v(const char* format, va_list ap)
    {
        std::string result;
        return string_append_v(result, format, ap);
    }

#ifndef __ANDROID__
    inline std::wstring string_format_v(const wchar_t* format, va_list ap)
    {
        std::wstring result;
        return string_append_v(result, format, ap);
    }
#endif

    inline std::string string_format(const char* format, ...)
    {
        std::string result;
        va_list ap;
        va_start(ap, format);
        string_append_v(result, format, ap);
        va_end(ap);
        return result;
    }

#ifndef __ANDROID__
    inline std::wstring string_format(const wchar_t* format, ...)
    {
        std::wstring result;
        va_list ap;
        va_start(ap, format);
        string_append_v(result, format, ap);
        va_end(ap);
        return result;
    }
#endif


} // utility

} // gorilla

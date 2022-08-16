/******************************************************************************

20141028 Seanni
   Use boost to replace string formatting


Description:

    Need boost/config.hpp (string_format.h) for BOOST_WINDOWS

*******************************************************************************/
#include "gorilla/utility/string_format.h"
#include <vector>
#include <stdio.h>
#include <stdexcept>


// #include <boost/config.hpp>

namespace gorilla
{

namespace utility
{
    // Function  Template Instantiation

    // template void string_append_vt<std::string>(std::string &str, const char
    // *format, va_list ap);
    //
    //#ifndef __ANDROID__
    // template void string_append_vt<std::wstring>(std::wstring &str, const
    // wchar_t *format, va_list ap);
    //#endif

    // same name for both character types

    static inline int vsnprintfT(
        char* buffer, size_t buf_size, const char* format, va_list argptr)
    {
#ifdef BOOST_WINDOWS
        return ::vsprintf_s(buffer, buf_size, format, argptr);
#else
        return ::vsnprintf(buffer, buf_size, format, argptr);
#endif
    }

#ifndef __ANDROID__
    static inline int vsnprintfT(
        wchar_t* buffer, size_t buf_size, const wchar_t* format, va_list argptr)
    {
#ifdef BOOST_WINDOWS
        return ::vswprintf_s(buffer, buf_size, format, argptr);
#else
        return ::vswprintf(buffer, buf_size, format, argptr);
#endif
    }
#endif

#define INIT_SIZE 1024

    template <class StringType>
    StringType& string_append_vt(StringType& dst,
        const typename StringType::value_type* format, va_list ap)
    {
        // First try with a small fixed size buffer.
        // This buffer size should be kept in sync with
        // StringUtilTest.GrowBoundary
        // and StringUtilTest.StringPrintfBounds.
        typename StringType::value_type stack_buf[INIT_SIZE];
        int result;

        // keep ap as it's original state, use copy only

        {
            va_list ap_copy;
#if defined(BOOST_WINDOWS)
#if defined(_MSC_VER)
#if _MSC_VER > 1700
            // VS2013 or above
            va_copy(ap_copy, ap); // available after VS2013
#else
            // VS2012
            ap_copy = ap;
#endif
#endif
#else
            errno = 0;
#endif
            result = vsnprintfT(stack_buf, INIT_SIZE, format, ap_copy);
            va_end(ap_copy);
        }

        if (result >= 0 && result < INIT_SIZE) {
            // It fit.
            dst.append(stack_buf, result);
            return dst;
        }

        // Repeatedly increase buffer size until it fits.
        int mem_length = INIT_SIZE;
        for (;;) {
            if (result < 0) {
#if defined(BOOST_WINDOWS)
                // On Windows, vsnprintfT always returns the number of
                // characters in a
                // fully-formatted string, so if we reach this point, something
                // else is
                // wrong and no amount of buffer-doubling is going to fix it.
                throw std::runtime_error("vsnprintf() failed");
#else
                if (errno != 0 && errno != EOVERFLOW) {
                    throw std::runtime_error("vsnprintf() failed");
                }
                // Try doubling the buffer size.
                mem_length *= 2;
#endif
            } else {
                // We need exactly "result + 1" characters.
                mem_length = result + 1;
            }

            if (mem_length > 32 * 1024 * 1024) {
                // That should be plenty, don't try anything larger.  This
                // protects
                // against huge allocations when using vsnprintfT
                // implementations that
                // return -1 for reasons other than overflow without setting
                // errno.
                // return ;
                throw std::runtime_error("string length is more than 32MB");
            }

            std::vector<typename StringType::value_type> mem_buf(mem_length);

            // NOTE: You can only use a va_list once.  Since we're in a while
            // loop, we
            // need to make a new copy each time so we don't use up the
            // original.
            {
                va_list ap_copy;
#if defined(BOOST_WINDOWS)
#if defined(_MSC_VER)
#if _MSC_VER > 1700
                // VS2013 or above
                va_copy(ap_copy, ap); // available after VS2013
#else
                // VS2012
                ap_copy = ap;
#endif
#endif
#else
                errno = 0;
#endif
                result = vsnprintfT(&mem_buf[0], mem_length, format, ap_copy);
                va_end(ap_copy);
            }

            if ((result >= 0) && (result < mem_length)) {
                // It fit.
                dst.append(&mem_buf[0], result);
                break; // for
            }
        }
        return dst;
    }


    std::string& string_append_v(
        std::string& dst, const char* format, va_list ap)
    {
        return string_append_vt<std::string>(dst, format, ap);
    }

#ifndef __ANDROID__
    std::wstring& string_append_v(
        std::wstring& dst, const wchar_t* format, va_list ap)
    {
        return string_append_vt<std::wstring>(dst, format, ap);
    }
#endif


} // utility

} // gorilla

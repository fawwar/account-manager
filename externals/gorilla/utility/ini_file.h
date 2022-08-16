/******************************************************************************
Notice:
*******************************************************************************/
#pragma once

#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>

// class CTextLines;
// class CINISection;
// class CINIKeyValuePair;
//

namespace gorilla
{
namespace utility
{

    class ini_file : public boost::property_tree::ptree
    {
    public:
        ini_file(void);
        ini_file(const ini_file& other);
        ini_file& operator=(const ini_file& other);
        virtual ~ini_file(void);

        void set_locale(const std::locale& loc);

        // clear() is provided by ptree
        bool append_from(const ini_file& other); // add/replace settings

        // bool load_from_file(const char * szINIFile);        // will clear
        // current content
        bool load_from_file(
            const char* szINIFile); // will clear current content
        bool load_from_string(std::istream& stream); // add/replace settings
        bool save_to_file(const char* szINIFile);
        bool save_to_stream(std::ostream& stream, bool bWriteBOM);

        // bool UpdateFromFile(CFile *pFile);

        std::string get_string(const char* szSection, const char* szKey,
            const char* szDefault) const;
        int get_integer(
            const char* szSection, const char* szKey, int nDefault) const;
        double get_double(const char* szSection, const char* szKey,
            double dblDefault = 0.0) const;

        void get_string(
            const char* tszSection, const char* tszKey, const char* szValue);
        void set_integer(
            const char* tszSection, const char* tszKey, int nValue);
        void set_double(
            const char* tszSection, const char* tszKey, double dblValue);

        void delete_key(const char* tszSection, const char* tszKey);

        std::string get_string_format_key(const char* szDefault,
            const char* tszSection, const char* tszKeyFormat, ...) const;
        int get_integer_format_key(int nDefault, const char* tszSection,
            const char* tszKeyFormat, ...) const;
        double get_double_format_key(double dblDefault, const char* tszSection,
            const char* tszKeyFormat, ...) const;

        void set_string_format_key(const char* szValue, const char* tszSection,
            const char* tszKeyFormat, ...);
        void set_integer_format_key(
            int nValue, const char* tszSection, const char* tszKeyFormat, ...);
        void set_double_format_key(double dblValue, const char* tszSection,
            const char* tszKeyFormat, ...);

    protected:
        const boost::property_tree::ptree* get_section(
            const char* szSection) const;
        boost::property_tree::ptree* get_section(const char* szSection);

    protected:
        std::locale m_locINI; // default to UTF-8
    };

} // utility
} // gorilla

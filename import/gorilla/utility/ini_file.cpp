/******************************************************************************

20141022 Seanni
   Use boost to replace implementation


Description:

    Section and key matching is case-insensitive

*******************************************************************************/
#include "gorilla/utility/ini_file.h"
#include "gorilla/utility/string_format.h"
#include "gorilla/utility/string_helper.h"
#include "gorilla/log/logger.h"

#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/ini_parser.hpp>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

namespace gorilla
{
namespace utility
{


    using namespace boost::property_tree;
    using namespace boost::algorithm;
    using namespace gorilla::log;

    typedef boost::property_tree::ptree ptree;

    /////////////////////////////////////////////////////////////////////////////
    //
    // ini_file
    //
    /////////////////////////////////////////////////////////////////////////////
    ini_file::ini_file()
    {
        m_locINI = boost::locale::generator()("en_US.UTF-8");
    }

    ini_file::ini_file(const ini_file& other)
    {
        this->operator=(other);
    }

    ini_file& ini_file::operator=(const ini_file& other)
    {
        clear();
        m_locINI = other.m_locINI;
        append_from(other);
        return *this;
    }

    ini_file::~ini_file()
    {
        clear();
    }

    void ini_file::set_locale(const std::locale& loc)
    {
        m_locINI = loc;
    }

    bool ini_file::append_from(const ini_file& other)
    {
        const_iterator it;

        for (it = other.begin(); it != other.end(); it++) {
            ptree::insert(ptree::end(), *it);
        }

        return true;
    }

    bool ini_file::load_from_file(const char* szINIFile)
    {
        try {
            read_ini(szINIFile, *(ptree*) this, m_locINI);
        } catch (std::exception& e) {
            clear();
            LOGGER_S(error) << e.what();
            return false;
        }
        return true;
    }

    bool ini_file::load_from_string(std::istream& stream)
    {
        try {
            read_ini(stream, *(ptree*) this);
        } catch (std::exception& e) {
            clear();
            LOGGER_S(error) << e.what();
            return false;
        }
        return true;
    }


    bool ini_file::save_to_file(const char* szINIFile)
    {
        try {
            write_ini(szINIFile, *(ptree*) this, 0, m_locINI);
        } catch (std::exception& e) {
            clear();
            LOGGER_S(error) << e.what();
            return false;
        }
        return true;
    }

    bool ini_file::save_to_stream(std::ostream& stream, bool bWriteBOM)
    {
        static const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
        try {
            if (bWriteBOM) {
                stream.write((char*) bom, 3);
            }
            write_ini(stream, *this, 0);
        } catch (std::exception& e) {
            clear();
            LOGGER_S(error) << e.what();
            return false;
        }
        return true;
    }

    const boost::property_tree::ptree* ini_file::get_section(
        const char* szSection) const
    {
        std::string strSection(szSection ? szSection : "");
        trim(strSection);
        if (strSection.empty()) {
            return (ptree*) this;
        }
        const_assoc_iterator assoc = this->find(strSection);
        if (assoc == not_found())
            return NULL;

        return &(this->to_iterator(assoc)->second);
    }

    boost::property_tree::ptree* ini_file::get_section(const char* szSection)
    {
        std::string strSection(szSection ? szSection : "");
        trim(strSection);
        if (strSection.empty()) {
            return (ptree*) this;
        }
        assoc_iterator assoc = this->find(strSection);
        if (assoc == not_found())
            return NULL;

        return &(this->to_iterator(assoc)->second);
    }

    std::string ini_file::get_string(
        const char* szSection, const char* szKey, const char* szDefault) const
    {
        const boost::property_tree::ptree* pSection = get_section(szSection);
        if (pSection == NULL)
            return std::string(szDefault);

        std::string strKey(szKey);
        trim(strKey);
        const_assoc_iterator assoc = pSection->find(strKey);
        if (assoc == pSection->not_found()) {
            return std::string(szDefault);
        }

        std::string strValue;
        strValue = pSection->to_iterator(assoc)->second.get_value<std::string>(
            szDefault);
        return strValue;
    }

    int ini_file::get_integer(
        const char* szSection, const char* szKey, int nDefault) const
    {
        const boost::property_tree::ptree* pSection = get_section(szSection);
        if (pSection == NULL)
            return nDefault;

        std::string strKey(szKey);
        trim(strKey);
        const_assoc_iterator assoc = pSection->find(strKey);
        if (assoc == pSection->not_found()) {
            return nDefault;
        }

        return pSection->to_iterator(assoc)->second.get_value(nDefault);
    }

    double ini_file::get_double(
        const char* szSection, const char* szKey, double dblDefault) const
    {
        const boost::property_tree::ptree* pSection = get_section(szSection);
        if (pSection == NULL)
            return dblDefault;

        std::string strKey(szKey);
        trim(strKey);
        const_assoc_iterator assoc = pSection->find(strKey);
        if (assoc == pSection->not_found()) {
            return dblDefault;
        }

        return pSection->to_iterator(assoc)->second.get_value<double>(
            dblDefault);
    }

    void ini_file::get_string(
        const char* szSection, const char* szKey, const char* szValue)
    {
        std::string strSection(szSection ? szSection : "");
        std::string strKey(szKey);

        trim(strSection);
        trim(strKey);

        if (strSection.empty()) {
            this->push_front(std::make_pair(strKey,
                ptree(szValue))); // put to front, otherwise write_ini will
                                  // output it into the last section
        } else {
            this->put(path_type(strSection + '/' + strKey, '/'), szValue);
        }
    }

    void ini_file::set_integer(
        const char* szSection, const char* szKey, int nValue)
    {
        std::string strSection(szSection ? szSection : "");
        std::string strKey(szKey);

        trim(strSection);
        trim(strKey);

        if (strSection.empty()) {
            ptree data;
            data.put_value(nValue);
            this->push_front(std::make_pair(strKey, data)); // put to front,
                                                            // otherwise
                                                            // write_ini will
                                                            // output it into
                                                            // the last section
        } else {
            this->put(path_type(strSection + '/' + strKey, '/'), nValue);
        }
    }

    void ini_file::set_double(
        const char* szSection, const char* szKey, double dblValue)
    {
        std::string strSection(szSection ? szSection : "");
        std::string strKey(szKey);

        trim(strSection);
        trim(strKey);

        if (strSection.empty()) {
            ptree data;
            data.put_value(dblValue);
            this->push_front(std::make_pair(strKey, data)); // put to front,
                                                            // otherwise
                                                            // write_ini will
                                                            // output it into
                                                            // the last section
        } else {
            this->put(path_type(strSection + '/' + strKey, '/'), dblValue);
        }
    }

    void ini_file::delete_key(const char* szSection, const char* szKey)
    {
        boost::property_tree::ptree* pSection = get_section(szSection);
        if (pSection == NULL) {
            return;
        }

        std::string strKey(szKey);
        trim(strKey);
        assoc_iterator assoc = pSection->find(strKey);
        if (assoc == pSection->not_found()) {
            return;
        }
        pSection->erase(pSection->to_iterator(assoc));
    }

    std::string ini_file::get_string_format_key(const char* szDefault,
        const char* szSection, const char* szKeyFormat, ...) const
    {
        va_list args;
        va_start(args, szKeyFormat);
        std::string strKeyName = string_format_v(szKeyFormat, args);
        va_end(args);
        return get_string(szSection, strKeyName.c_str(), szDefault);
    }

    int ini_file::get_integer_format_key(
        int nDefault, const char* szSection, const char* szKeyFormat, ...) const
    {
        va_list args;
        va_start(args, szKeyFormat);
        std::string strFormattedKey = string_format_v(szKeyFormat, args);
        va_end(args);
        return get_integer(szSection, strFormattedKey.c_str(), nDefault);
    }

    double ini_file::get_double_format_key(double dblDefault,
        const char* szSection, const char* szKeyFormat, ...) const
    {
        va_list args;
        va_start(args, szKeyFormat);
        std::string strFormattedKey = string_format_v(szKeyFormat, args);
        va_end(args);
        return get_double(szSection, strFormattedKey.c_str(), dblDefault);
    }

    void ini_file::set_string_format_key(const char* szValue,
        const char* szSection, const char* szKeyFormat, ...)
    {
        va_list args;
        va_start(args, szKeyFormat);
        std::string strFormattedKey = string_format_v(szKeyFormat, args);
        va_end(args);
        get_string(szSection, strFormattedKey.c_str(), szValue);
    }

    void ini_file::set_integer_format_key(
        int nValue, const char* szSection, const char* szKeyFormat, ...)
    {
        va_list args;
        va_start(args, szKeyFormat);
        std::string strFormattedKey = string_format_v(szKeyFormat, args);
        va_end(args);
        set_integer(szSection, strFormattedKey.c_str(), nValue);
    }

    void ini_file::set_double_format_key(
        double dblValue, const char* szSection, const char* szKeyFormat, ...)
    {
        va_list args;
        va_start(args, szKeyFormat);
        std::string strFormattedKey = string_format_v(szKeyFormat, args);
        va_end(args);
        set_double(szSection, strFormattedKey.c_str(), dblValue);
    }

    // void ini_file::GetSectionNames(CStringArray &strSections) const
    //{
    //    strSections.RemoveAll();
    //    POSITION pos = m_listSections.GetHeadPosition();
    //    while(pos!=NULL)
    //    {
    //        const CINISection *pSection = m_listSections.GetNext(pos);
    //        strSections.Add(pSection->GetSectionName());
    //    }
    //}
    //
    // void ini_file::GetSectionValues(const char * szSection, CINIStringArray
    // &strKeys, CINIStringArray &strValues) const
    //{
    //    ASSERT(strKeys.GetCount() == strValues.GetCount());
    //    const CINISection *pSection = get_section(szSection);
    //    if (pSection == NULL)
    //        return;
    //    pSection->GetKeyValueArray(strKeys, strValues);
    //}
    //
    //// assumed that the section is not there already
    // CINISection *ini_file::CreateSection(const char * szSection, const char *
    // szLine)
    //{
    //    CINISection *pSection = NULL;
    //    CStringW strSECTION;
    //    try
    //    {
    //        pSection = new CINISection(szSection, szLine);
    //        m_listSections.AddTail(pSection);
    //        strSECTION = szSection;
    //        strSECTION.MakeUpper();
    //        m_mapSections.SetAt(strSECTION, pSection);
    //    }
    //    catch(CException *pE)
    //    {
    //        pE->Delete();
    //        if (pSection != NULL)
    //        {
    //            DeleteSectionListEntry(pSection);
    //            m_mapSections.RemoveKey(strSECTION);
    //            delete pSection;
    //        }
    //        return NULL;
    //    }
    //    return pSection;
    //}
    //
    // bool ini_file::DeleteSectionListEntry(CINISection *pSection)
    //{
    //    POSITION posList = m_listSections.Find(pSection);
    //    if (posList != NULL)
    //    {
    //        m_listSections.RemoveAt(posList);
    //        return true;
    //    }
    //    return false;
    //}
    //
    // const CINISection *ini_file::get_section(const char * szSection) const
    //{
    //    CINISection *pSection;
    //    CStringW strSECTION = szSection;
    //    strSECTION.MakeUpper();
    //    if (m_mapSections.Lookup(strSECTION, pSection))
    //    {
    //        return pSection;
    //    }
    //    return NULL;
    //}
    //
    //
    // CINISection *ini_file::get_section(const char * szSection)
    //{
    //    CINISection *pSection;
    //    CStringW strSection = szSection;
    //    strSection.MakeUpper();
    //    if (m_mapSections.Lookup(strSection, pSection))
    //    {
    //        return pSection;
    //    }
    //    return NULL;
    //}


    /////////////////////////////////////////////////////////////////////////////
    //
    // CINISection
    //
    /////////////////////////////////////////////////////////////////////////////
    //
    // CINISection::CINISection(const char * szSection, const char * szLine) :
    //    m_wstrSection(szSection), m_wstrSectionLine(szLine)
    //{
    //    if (m_wstrSectionLine.IsEmpty())
    //    {
    //        CT2CW wstrSection(szSection);
    //        m_wstrSectionLine.Format(L"[%s]", (const char *)wstrSection);
    //    }
    //}
    //
    // CINISection::CINISection(const CINISection &other)
    //{
    //    this->operator=(other);
    //}
    //
    // CINISection &CINISection::operator=(const CINISection &other)
    //{
    //    CINIKeyValuePair *pPair = NULL;
    //    const CINIKeyValuePair *pOtherPair;
    //
    //    clear();
    //
    //    m_wstrSection = other.m_wstrSection;
    //    m_wstrSectionLine = other.m_wstrSectionLine;
    //
    //    POSITION posList = other.m_listPairs.GetHeadPosition();
    //    while(posList != NULL)
    //    {
    //        CStringW strKEY; // to make it upper
    //        pOtherPair = other.m_listPairs.GetNext(posList);
    //        try
    //        {
    //            pPair = new CINIKeyValuePair(*pOtherPair);
    //            strKEY = pPair->GetKey();
    //            strKEY.MakeUpper();
    //            m_listPairs.AddTail(pPair);
    //            m_mapPairs.SetAt(strKEY, pPair);
    //        }
    //        catch(...)
    //        {
    //            if (pPair != NULL)
    //            {
    //                DeletePairListEntry(pPair);
    //                m_mapPairs.RemoveKey(strKEY);
    //                delete pPair;
    //            }
    //            throw;
    //        }
    //        pPair = NULL;
    //    }
    //
    //    return *this;
    //}
    //
    // CINISection::~CINISection()
    //{
    //    clear();
    //}
    //
    // void CINISection::clear()
    //{
    //    CINIKeyValuePair *pPair;
    //    POSITION posList = m_listPairs.GetHeadPosition();
    //    while(posList != NULL)
    //    {
    //        pPair = m_listPairs.GetNext(posList);
    //        delete pPair;
    //    }
    //    m_listPairs.RemoveAll();
    //    m_mapPairs.RemoveAll();
    //}
    //
    // void CINISection::UpdateFrom(const CINISection &other)
    //{
    //    CINIKeyValuePair *pOtherPair;
    //    POSITION posOtherPairList = other.m_listPairs.GetHeadPosition();
    //    while(posOtherPairList != NULL)
    //    {
    //        pOtherPair = other.m_listPairs.GetNext(posOtherPairList);
    //        SetValue(pOtherPair->GetKey(), pOtherPair->GetValue());
    //    }
    //}
    //
    // const char * CINISection::GetValue(const char * szKey, const char *
    // szDefault) const
    //{
    //    CINIKeyValuePair *pPair;
    //    CStringW strKEY=szKey;
    //    strKEY.MakeUpper();
    //    if (m_mapPairs.Lookup(strKEY, pPair))
    //    {
    //        return pPair->GetValue();
    //    }
    //    return szDefault;
    //}
    //
    // void CINISection::GetKeyValueArray(CINIStringArray &strKeys,
    // CINIStringArray &strValues) const
    //{
    //    ASSERT(strKeys.GetCount() == strValues.GetCount());
    //    const CINIKeyValuePair *pPair;
    //    POSITION posList = m_listPairs.GetHeadPosition();
    //    while(posList != NULL)
    //    {
    //        pPair = m_listPairs.GetNext(posList);
    //        strKeys.Add(pPair->GetKey());
    //        strValues.Add(pPair->GetValue());
    //    }
    //}
    //
    // CINIKeyValuePair *CINISection::GetPair(const char * szKey) const
    //{
    //    CINIKeyValuePair *pPair;
    //    CStringW strKEY = szKey;
    //    strKEY.MakeUpper();
    //
    //    if (m_mapPairs.Lookup(strKEY, pPair))
    //        return pPair;
    //    return NULL;
    //}
    //
    //
    // CINIKeyValuePair *CINISection::CreatePair(const char * szKey, const char
    // * wszValue, const char * wszLine)
    //{
    //    CINIKeyValuePair *pPair = NULL;
    //    CStringW strKEY;
    //    ASSERT(szKey != NULL);
    //    try
    //    {
    //        pPair = new CINIKeyValuePair(szKey, wszValue, wszLine);
    //        m_listPairs.AddTail(pPair);
    //        strKEY = szKey;
    //        strKEY.MakeUpper();
    //        m_mapPairs.SetAt(strKEY, pPair);
    //    }
    //    catch(CException *pE)
    //    {
    //        pE->Delete();
    //        if (pPair != NULL)
    //        {
    //            DeletePairListEntry(pPair);
    //            m_mapPairs.RemoveKey(strKEY);
    //            delete pPair;
    //        }
    //        return NULL;
    //    }
    //    return pPair;
    //}
    //
    //// should never be a empty line
    //// '\r\n' has been removed
    // void CINISection::ParsePairLine(const CStringW &wstrLine)
    //{
    //    CStringW wstrKey;
    //    CStringW wstrValue;
    //    CStringW wstrKEY;
    //    CINIKeyValuePair *pPair=NULL;
    //
    //    try
    //    {
    //        int iEqual = wstrLine.Find(L'=');
    //        if (iEqual == -1 || iEqual == 0)
    //        {
    //            // just keep the line
    //            pPair = new CINIKeyValuePair(NULL, NULL, wstrLine);
    //            // no key, just the line
    //            m_listPairs.AddTail(pPair);
    //            return;
    //        }
    //
    //        wstrKey = wstrLine.Left(iEqual);
    //        wstrKey.Trim();
    //        wstrValue = ((const char *) wstrLine) + (iEqual + 1);
    //        wstrValue.Trim();
    //
    ////        TRACE(_T("Section[%s] Key[%s] Value[%s]\n"), m_wstrSection,
    ///strKey, strValue);
    //        CW2CT strKey(wstrKey);
    //        pPair = GetPair(strKey);
    //        if (pPair == NULL)
    //        {
    //            pPair = CreatePair(strKey, wstrValue, wstrLine);
    //        } else
    //        {
    //            pPair->ReplaceValue(wstrValue);
    //        }
    //    }
    //    catch(CException *pE)
    //    {
    //        pE->Delete();
    //        if (pPair != NULL)
    //        {
    //            DeletePairListEntry(pPair);
    //            delete pPair;
    //        }
    //    }
    //}
    //
    // void CINISection::SetValue(const char * szKey, const char * wszValue)
    //{
    //    CINIKeyValuePair *pPair = GetPair(szKey);
    //    if (pPair == NULL)
    //    {
    //        pPair = CreatePair(szKey, wszValue, NULL);
    //        if (pPair == NULL)
    //            return;        // bad thing happen
    //    } else
    //    {
    //        pPair->ReplaceValue(wszValue);
    //    }
    //}
    //
    // bool CINISection::delete_key(const char * szKey)
    //{
    //    CINIKeyValuePair *pPair=NULL;
    //    CStringW strKEY = szKey;
    //    strKEY.MakeUpper();
    //    if (m_mapPairs.Lookup(strKEY, pPair))
    //    {
    //        m_mapPairs.RemoveKey(strKEY);
    //        DeletePairListEntry(pPair);
    //        delete pPair;
    //        return true;
    //    }
    //    return false;
    //}
    //
    // void CINISection::WriteLines(CTextLines &lines)
    //{
    //    lines.InsertLine(m_wstrSectionLine);
    //
    //    CINIKeyValuePair *pPair;
    //    POSITION posList = m_listPairs.GetHeadPosition();
    //    while(posList != NULL)
    //    {
    //        pPair = m_listPairs.GetNext(posList);
    //
    //        pPair->WriteLines(lines);
    //    }
    //}
    //
    // bool CINISection::DeletePairListEntry(CINIKeyValuePair *pPair)
    //{
    //    POSITION posList = m_listPairs.Find(pPair);
    //    if (posList != NULL)
    //    {
    //        m_listPairs.RemoveAt(posList);
    //        return true;
    //    }
    //    return false;
    //}
    //


} // utility

} // gorilla

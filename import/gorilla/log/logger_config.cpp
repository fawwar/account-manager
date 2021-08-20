#include "gorilla/log/logger_config.h"
#include "gorilla/utility/ini_file.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

namespace gorilla
{
   namespace log
   {

using namespace gorilla::utility;

logger_config::~logger_config()
{
   if (!m_bApplied)
   {
      apply();
   }
}

bool logger_config::load( const boost::filesystem::path &pathConfig, const char *szSection /*= LOGGER_CONFIG_INI_SECTION*/ )
{
   ini_file ini;
   bool bRet = ini.load_from_file(pathConfig.string().c_str());
   load_common_conf(ini, szSection);
   load_file_conf(ini, szSection);
   load_console_conf(ini, szSection);
   load_debugger_conf(ini, szSection);
   load_syslog_conf(ini, szSection);

   return bRet;
}

void logger_config::apply()
{
   apply_common();
   apply_file();
   apply_console();
   apply_debugger();
   apply_syslog();
   m_bApplied = true;
}

void logger_config::load_common_conf(const gorilla::utility::ini_file &ini, const char *szSection )
{
   m_bUTC = ini.get_integer(szSection, "UTC", m_bUTC ? 1 : 0) ? true : false;
}

void logger_config::load_file_conf(const gorilla::utility::ini_file &ini, const char *szSection )
{
   std::string str;
   str = ini.get_string(szSection, "FileLogger.Severity", "");
   if (str.length())
   {
      string_to_severity(str, m_eFileSeverity);
   }
   m_strFileChannels = ini.get_string(szSection, "FileLogger.Channels", m_strFileChannels.c_str());
   m_bFileAutoFlush = ini.get_integer(szSection, "FileLogger.AutoFlush", m_bFileAutoFlush ? 1 : 0) ? true : false;

   str=ini.get_string(szSection, "FileLogger.LogFilePattern", "");
   if (str.length())
   {
      // UTF-8 to wide char
      std::wstring wstr = boost::locale::conv::to_utf<wchar_t>(str, "UTF-8");
      m_wpathLogFilePattern = wstr;
   }
   // m_strFileNamePattern = ini.get_string(szSection, "FileLogger.FileNamePrefix", m_strFileNamePattern.c_str());
   m_nFileRotationMaxSizeK = ini.get_integer(szSection, "FileLogger.Rotation.MaxSizeK", m_nFileRotationMaxSizeK);
   m_nFileStorageMaxUsedK =  ini.get_integer(szSection, "FileLogger.Storage.MaxUsedK", m_nFileStorageMaxUsedK);
   m_nFileStorageMinFreeK =  ini.get_integer(szSection, "FileLogger.Storage.MinFreeK", m_nFileStorageMinFreeK);
}

void logger_config::load_console_conf(const gorilla::utility::ini_file &ini, const char *szSection )
{
   std::string str;
   str = ini.get_string(szSection, "ConsoleLogger.Severity", "");
   if (str.length())
   {
      string_to_severity(str, m_eConsoleSeverity);
   }
   m_strConsoleChannels = ini.get_string(szSection, "ConsoleLogger.Channels", m_strConsoleChannels.c_str());
}

void logger_config::load_debugger_conf(const gorilla::utility::ini_file &ini, const char *szSection )
{
   std::string str;
   str = ini.get_string(szSection, "DebuggerLogger.Severity", "");
   if (str.length())
   {
      string_to_severity(str, m_eDebuggerSeverity);
   }
   m_strDebuggerChannels = ini.get_string(szSection, "DebuggerLogger.Channels", m_strDebuggerChannels.c_str());
   m_bDebuggerInDebuggerOnly = ini.get_integer(szSection, "DebuggerLogger.InDebuggerOnly", m_bDebuggerInDebuggerOnly ? 1 : 0) ? true : false;
}

void logger_config::load_syslog_conf(const gorilla::utility::ini_file &ini, const char *szSection )
{
   std::string str;
   str = ini.get_string(szSection, "SyslogLogger.Severity", "");
   if (str.length())
   {
      string_to_severity(str, m_eSyslogSeverity);
   }
   m_strSyslogChannels = ini.get_string(szSection, "SyslogLogger.Channels", m_strSyslogChannels.c_str());
   m_strSyslogFacility = ini.get_string(szSection, "SyslogLogger.Facility", m_strSyslogFacility.c_str());

   m_strSyslogTargetHost = ini.get_string(szSection, "SyslogLogger.TargetIP", m_strSyslogTargetHost.c_str());
   m_nSyslogTargetPort = (unsigned short)ini.get_integer(szSection, "SyslogLogger.TargetPort", m_nSyslogTargetPort);

   m_strSyslogLocalHost = ini.get_string(szSection, "SyslogLogger.LocalIP", m_strSyslogLocalHost.c_str());
   m_nSyslogLocalPort = (unsigned short)ini.get_integer(szSection, "SyslogLogger.LocalPort", m_nSyslogLocalPort);
}

bool logger_config::string_to_severity(std::string const &str, SL &eSeverity)
{
   if (boost::iequals(str, "trace") || str == "0")
   {
      eSeverity = trace;
   } else if (boost::iequals(str, "debug") || str == "1")
   {
      eSeverity = debug;
   } else if (boost::iequals(str, "info") || str == "2")
   {
      eSeverity = info;
   } else if (boost::iequals(str, "warning") || boost::iequals(str, "warn") || str == "3")
   {
      eSeverity = warning;
   } else if (boost::iequals(str, "error") || str == "4")
   {
      eSeverity = error;
   } else if (boost::iequals(str, "fatal") || str == "5")
   {
      eSeverity = fatal;
   } else if (boost::iequals(str, "never") || str == "6")
   {
      eSeverity = never;
   } else
   {
      return false;
   }
   return true;
}

void logger_config::apply_common()
{
   logger_keeper *pKeeper = logger_keeper::get_instance();
   pKeeper->set_utc_timestamp(m_bUTC);
}

void logger_config::apply_file()
{
   logger_keeper *pKeeper = logger_keeper::get_instance();
   boost::filesystem::path path= boost::filesystem::absolute(m_wpathLogFilePattern);
   pKeeper->set_file_pattern(path.string().c_str());

   pKeeper->set_file_severity_level(m_eFileSeverity);
   pKeeper->set_file_channels(m_strFileChannels.c_str());
   pKeeper->set_file_rotation_size((uintmax_t)1024 * m_nFileRotationMaxSizeK);
   pKeeper->set_file_storage_size((uintmax_t)1024 * m_nFileStorageMaxUsedK, (uintmax_t)1024 * m_nFileStorageMinFreeK);
   pKeeper->set_file_auto_flush(m_bFileAutoFlush);
}

void logger_config::apply_console()
{
   logger_keeper *pKeeper = logger_keeper::get_instance();
   pKeeper->set_console_severity_level(m_eFileSeverity);
   pKeeper->set_console_channels(m_strConsoleChannels.c_str());
}

void logger_config::apply_debugger()
{
   logger_keeper *pKeeper = logger_keeper::get_instance();
   pKeeper->set_debugger_severity_level(m_eDebuggerSeverity);
   pKeeper->set_debugger_channels(m_strDebuggerChannels.c_str());
   pKeeper->set_debugger_only_in_debugger(m_bDebuggerInDebuggerOnly);
}

void logger_config::apply_syslog()
{
   logger_keeper *pKeeper = logger_keeper::get_instance();
   pKeeper->set_syslog_facility(m_strSyslogFacility.c_str());

   pKeeper->set_syslog_severity_level(m_eSyslogSeverity);
   pKeeper->set_syslog_channels(m_strSyslogChannels.c_str());

   pKeeper->set_syslog_target_address(m_strSyslogTargetHost.c_str(), m_nSyslogTargetPort);
   pKeeper->set_syslog_local_address(m_strSyslogLocalHost.c_str(), m_nSyslogLocalPort);
}


   }
}



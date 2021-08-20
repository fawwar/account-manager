#pragma once
#include "gorilla/log/logger.h"


namespace gorilla
{
   namespace utility
   {
      class ini_file;
   }


   namespace log
   {

#define LOGGER_LOG_FILE_PATTERN_DATE_TIME_SN       "%Y-%m-%d_%H-%M-%S.%2N.log"
#define LOGGER_LOG_FILE_PATTERN_DATE_TIME          "%Y-%m-%d_%H-%M-%S.log"
#define LOGGER_LOG_FILE_PATTERN_DATE_SN            "%Y-%m-%d-%3N.log"
#define LOGGER_LOG_FILE_PATTERN_DATE               "%Y-%m-%d.log"

// use pattern containing LOGGER_LOG_FILE_PATTERN_DATE if you want to append log to the same per-day log file

#define LOGGER_LOG_FILE_PATTERN_DEFAULT            "log\\log-%Y-%m-%d_%H-%M-%S.log"

#define WIDE(str) L##str

#define LOGGER_CONFIG_INI_SECTION                  "Log"

class logger_config
{
public:
   boost::filesystem::wpath m_wpathLogFilePattern;    // including sub-folder

   bool m_bUTC;

   // file logger
   SL m_eFileSeverity;
   std::string m_strFileChannels;
   int m_nFileRotationMaxSizeK;
   int m_nFileStorageMaxUsedK;
   int m_nFileStorageMinFreeK;
   bool m_bFileAutoFlush;

   // console logger
   SL m_eConsoleSeverity;
   std::string m_strConsoleChannels;

   // debugger logger
   SL m_eDebuggerSeverity;
   std::string m_strDebuggerChannels;
   bool m_bDebuggerInDebuggerOnly;

   //syslog logger
   SL m_eSyslogSeverity;
   std::string m_strSyslogChannels;

   std::string m_strSyslogFacility;
   std::string m_strSyslogTargetHost;
   unsigned short m_nSyslogTargetPort;
   std::string m_strSyslogLocalHost;
   unsigned short m_nSyslogLocalPort;

   bool m_bApplied;

public:
   logger_config()
      : m_bApplied(false)
   {
      reset();
   }

   logger_config(const boost::filesystem::wpath &wpath)
      : m_wpathLogFilePattern(wpath)
   {
      reset();
   }

   virtual ~logger_config();     // call apply() if 1m_bApplied

   void reset()
   {
      m_bUTC = false;
      m_eFileSeverity = error;
      m_bFileAutoFlush = false;
      m_nFileRotationMaxSizeK = 0;
      m_nFileStorageMaxUsedK = 0;
      m_nFileStorageMinFreeK = 0;
      m_eConsoleSeverity = never;
      m_eDebuggerSeverity = trace;
      m_bDebuggerInDebuggerOnly = true;

      m_strSyslogFacility = "user";
      m_eSyslogSeverity = error;
      m_strSyslogTargetHost ="127.0.0.1";
      m_nSyslogTargetPort = 814;
      m_strSyslogLocalHost = "";    // empty, will no call
      m_nSyslogLocalPort = 814;
   }

public:
   bool configure_with(const boost::filesystem::path &pathConfig, const char *szSection = LOGGER_CONFIG_INI_SECTION)
   {
      bool bRet = load(pathConfig, szSection);
      apply();
      return bRet;
   }
   bool load(const boost::filesystem::path &pathConfig, const char *szSection = LOGGER_CONFIG_INI_SECTION);
   // call apply after load/change configuration
   void apply();

protected:
   virtual void load_common_conf(const gorilla::utility::ini_file &ini, const char *szSection);
   virtual void load_file_conf(const gorilla::utility::ini_file &ini, const char *szSection);
   virtual void load_console_conf(const gorilla::utility::ini_file &ini, const char *szSection);
   virtual void load_debugger_conf(const gorilla::utility::ini_file &ini, const char *szSection);
   virtual void load_syslog_conf(const gorilla::utility::ini_file &ini, const char *szSection);

   virtual void apply_common();
   virtual void apply_file();
   virtual void apply_console();
   virtual void apply_debugger();
   virtual void apply_syslog();

   static bool string_to_severity(std::string const &str, SL &eSeverity);
};


   }
}



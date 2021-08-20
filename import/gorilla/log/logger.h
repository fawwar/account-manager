#pragma once
// https://svn.boost.org/trac/boost/wiki/Guidelines/WarningsGuidelines

#include <string>

#pragma warning( disable : 4819 )

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/lexical_cast.hpp>
#if defined(BOOST_WINDOWS)
#include <boost/log/sinks/debug_output_backend.hpp>
#endif
#if !defined(LOGGER_NO_SYSLOG)
#include <boost/log/sinks/syslog_backend.hpp>
#endif
#include <boost/format.hpp>



namespace gorilla
{
   namespace log
   {

// serverity_level
// keep the type name short to avoid warning C4503
enum SL
{
   trace,
   debug,
   info,
   warning,
   warn = warning,
   error,
   fatal,
   never    // for limit only, do not use the level for logging
};

// for support of wide char
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
   std::basic_ostream< CharT, TraitsT >& strm, SL lvl)
{
   static const char* const str[] =
   {
      "trace",
      "debug",
      "info",
      "warning",
      "error",
      "fatal",
      "never"
   };
   if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
      strm << str[lvl];
   else
      strm << boost::lexical_cast<std::basic_string<CharT, TraitsT>>(static_cast< int >(lvl));
   return strm;
}

// common across loggers
#define LOGGER_KEEPER_DEFAULT_USE_UTC_TIMESTAMP false

// file logger filename pattern (please prefix with folder name if necessary)
#define LOGGER_KEEPER_DEFAULT_LOG_FILE_PATTERN "log_%Y-%m-%d_%H-%M-%S.%2N.log"
// use "log_%Y-%m-%d_%H-%M-%S.log" to append to log file of the same day
//#define LOGGER_KEEPER_DEFAULT_LOG_FILE_PATTERN "log_%Y-%m-%d_%H-%M-%S.%2N.log"

#define LOGGER_KEEPER_DEFAULT_LOG_FILE_ROTATION_SIZE     10 * 1024 * 1024
#define LOGGER_KEEPER_DEFAULT_LOG_FILE_STORAGE_MAX_USED  1024 * 1024 * 1024
#define LOGGER_KEEPER_DEFAULT_LOG_FILE_STORAGE_MIN_SPACE 0
#define LOGGER_KEEPER_DEFAULT_LOG_FILE_ENABLE_AUTO_FLUSH false

// debugger logger
#define LOGGER_KEEPER_DEFAULT_IN_DEBUGGER_ONLY     true

// logger types
typedef boost::log::sources::severity_logger_mt<SL> logger_s_type;
typedef boost::log::sources::wseverity_logger_mt<SL> wlogger_s_type;
typedef boost::log::sources::severity_channel_logger_mt<SL, std::string> logger_sc_type;
typedef boost::log::sources::wseverity_channel_logger_mt<SL, std::string> wlogger_sc_type;

// Log sources
// Create global loggers
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(_logger_s, logger_s_type)
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(_wlogger_s, wlogger_s_type)
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(_logger_sc, logger_sc_type)
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(_wlogger_sc, wlogger_sc_type)

//
// Please use the following macros
//
#define LOGGER_S(severity) BOOST_LOG_STREAM_SEV(gorilla::log::_logger_s::get(), (severity))
#define WLOGGER_S(severity) BOOST_LOG_STREAM_SEV(gorilla::log::_wlogger_s::get(), (severity))
// default severity is info
#define LOGGER() LOGGER_S(info)
#define WLOGGER() WLOGGER_S(info)
// channel is narrow char
#define LOGGER_SC(severity, channel) BOOST_LOG_CHANNEL_SEV(gorilla::log::_logger_sc::get(), (channel), (severity))
#define WLOGGER_SC(severity, channel) BOOST_LOG_CHANNEL_SEV(gorilla::log::_wlogger_sc::get(), (channel), (severity))

// file log setting
typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_file_backend > sink_file;
typedef boost::shared_ptr<sink_file> sink_file_sp;
typedef boost::log::sinks::synchronous_sink< boost::log::sinks::wtext_ostream_backend> sink_console;
typedef boost::shared_ptr<sink_console> sink_console_sp;
#if defined(BOOST_WINDOWS)
typedef boost::log::sinks::synchronous_sink< boost::log::sinks::wdebug_output_backend > sink_debugger;
typedef boost::shared_ptr<sink_debugger> sink_debugger_sp;
#endif
#ifndef LOG_NO_SYSLOG
typedef boost::log::sinks::synchronous_sink< boost::log::sinks::syslog_backend > sink_syslog;
typedef boost::shared_ptr<sink_syslog> sink_syslog_sp;
#endif

//
//template<class T> class t_singleton
//{
//protected:
//   static boost::atomic<T *> s_pInstance;
//   static boost::mutex s_mutexInstantiation;
//
//protected:
//
//#if defined(BOOST_WINDOWS)
//   static void __cdecl destroy_at_exit(void);
//#else
//   static void destroy_at_exit(void);
//#endif
//
//public:
//   //
//   static T * get_instance()
//   {
//      T *ptr = s_pInstance.load();
//      if (ptr == NULL)
//      {
//         boost::mutex::scoped_lock lock(s_mutexInstantiation);
//         // again, after locked
//         ptr = s_pInstance.load();
//         if (ptr == NULL)
//         {
//            ptr = new T();
//            s_pInstance.store(ptr);
//            atexit(&destroy_at_exit);
//         }
//      }
//      return ptr;
//   }
//   static void destroy_instance()
//   {
//      T *ptr = s_pInstance.load();
//      delete ptr;
//      s_pInstance.store(NULL);
//   }
//};

class keeper_impl;

class logger_keeper
{
   // friend class t_singleton<logger_keeper>;
   friend class keeper_impl;
public:

protected:
   static boost::atomic<logger_keeper *> s_pInstance;
   static boost::mutex s_mutexInstantiation;

   keeper_impl *m_impl;

protected:  // should be created/destroyed by static members
   logger_keeper();
   virtual ~logger_keeper();

public:
   void add_all_loggers();
   void flush();
   void destroy();
   void set_utc_timestamp(bool bUTC);    // default is local time

   //
   // File logger
   //
   void add_file_logger();
   void remove_file_logger();
   bool is_file_logger_available() const;
   //
   // File logger setting
   //
   // the following methods could be called before add_file_logger()
   void set_file_pattern(const char *pattern);
   template< typename PathT > void set_file_pattern(PathT const & pattern);   // pattern could be std::string or boost::filesystem::path
   void get_pattern_parent_path(boost::filesystem::path &path) const;
   void set_file_rotation_size(uintmax_t max_size);    // size limit of current log file
   void set_file_storage_size(uintmax_t max_used, uintmax_t min_free);
   void set_file_auto_flush(bool flush);
   //
   // File logger filter setting
   //
   void set_file_severity_level(SL eSL);
   SL get_file_severity_level() const;
   void set_file_channels(const char *str = "");    // set channel filters (a list of channels separate by comma or space )
   const std::string &get_file_channels() const;
   // if further customization is needed
   sink_file_sp get_file_sink() const;    // http://www.boost.org/doc/libs/1_58_0/libs/log/doc/html/boost/log/sinks/text_file_backend.html


   //
   // Console logger
   //
   void add_console_logger();
   void remove_console_logger();
   bool is_console_logger_available() const;
   //
   // Console logger filter setting
   //
   void set_console_severity_level(SL eSL);
   SL get_console_severity_level() const;
   void set_console_channels(const char *str = "");     // set channel filters (a list of channels separate by comma or space )
   const std::string &get_console_channels() const;
   // if further customization is needed
   sink_console_sp &get_console_sink() const;

   //
   // Debugger logger
   //
   void add_debugger_logger();
   void remove_debugger_logger();
   bool is_debugger_logger_available() const;
   //
   // Debugger logger filter setting
   //
   void set_debugger_severity_level(SL eSL);
   SL get_debugger_severity_level() const;
   void set_debugger_channels(const char *str = "");     // set channel filters (a list of channels separate by comma or space )
   const std::string &get_debugger_channels() const;
   void set_debugger_only_in_debugger(bool only_in_debugger);
   bool get_debugger_only_in_debugger() const;
#if defined(BOOST_WINDOWS)
   // if further customization is needed
   sink_debugger_sp &get_debugger_sink() const;
#endif

   //
   // syslog logger
   //
   void add_syslog_logger();
   void remove_syslog_logger();
   bool is_syslog_logger_available() const;
   //
   // syslog logger setting
   //
   void set_syslog_facility(const char *facility); // called before calling add_syslog_logger
   void set_syslog_target_address(const char *host, unsigned short port = 514);
   void set_syslog_local_address(const char *host, unsigned short port = 514);      // for specifying source port
   //
   // syslog logger filter setting
   //
   void set_syslog_severity_level(SL eSL);
   SL get_syslog_severity_level() const;
   void set_syslog_channels(const char *str = "");     // set channel filters (a list of channels separate by comma or space )
   const std::string &get_syslog_channels() const;
   // if further customization is needed
#ifndef LOGGER_NO_SYSLOG
   sink_syslog_sp &get_syslog_sink() const;
#endif

public:

   // singleton
   static logger_keeper* get_instance();
   static void destroy_instance();
#ifdef BOOST_WINDOWS
   static void __cdecl destroy_at_exit(void);
#else
   static void destroy_at_exit(void);
#endif
};

static inline logger_keeper *get_keeper() { return logger_keeper::get_instance(); }

// use locked_backend() to access the pointer of sink backends

// When writing wide char string to WLOGGER, the wide char string will be converted to UTF-8 for output
// since the debugger can not show UTF-8, the line of log text will produce output of UTF-8



extern void logger_init();       // initialize logger_keeper object, same to call logger_keeper::get_keeper()
extern void logger_flush();
extern void logger_destroy();    // will also destroy the logger_keeper instance
   } // log

}  // gorilla



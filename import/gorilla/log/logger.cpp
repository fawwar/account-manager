/*

Revision History:
   20150811-0000 gorillani
      gcc compatibility
   20150721-0000 gorillani
      Add destroy_instance
   20150708-1200 gorillani
      Console sink uses wchar_t based ostream
   20150707-0000 gorillani
      Debugger sink uses wchar_t base sink

*/
#include "gorilla/log/logger.h"
#pragma warning(disable: 4512)   // https://svn.boost.org/trac/boost/ticket/10458
#pragma warning(disable: 4714)
//#pragma warning(disable: 4100)   // unreferenced formal parameter

#pragma warning(disable: 4100)   // unreferenced formal parameter                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           warning(disable: 4100)   // unreferenced formal parameter
#pragma warning(disable: 4995)

#include <boost/locale/generator.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/date_time/posix_time/posix_time_types.hpp>
//#include <boost/date_time/local_time/local_date_time.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/attributes/value_extraction.hpp>
// #include <boost/log/attributes/attribute_name.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/detail/default_attribute_names.hpp>
#if !defined(BOOST_WINDOWS)
#include <boost/log/sinks/syslog_constants.hpp>
#endif
#include <limits>


//
// http://www.boost.org/doc/libs/1_58_0/libs/log/doc/html/boost/log/sinks/text_file_backend.html
//


namespace gorilla
{
   namespace log
   {

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;


// Declare attribute keywords
// BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SL);
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime);       // the time type is fixed, see also: add_common_attributes()
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string);

#ifndef LOGGER_NO_SYSLOG
static boost::log::sinks::syslog::facility syslog_string_to_facility(const char *facility);
#endif

//
// implementation of logger_keeper
//
class keeper_impl
{
   friend class logger_keeper;
protected:

   boost::shared_ptr< boost::log::core > m_log_core_sp;
   sink_file_sp m_file_sink_sp;
   sink_console_sp m_console_sink_sp;
#if defined(BOOST_WINDOWS)
   sink_debugger_sp m_debugger_sink_sp;
#endif
#ifndef LOGGER_NO_SYSLOG
   sink_syslog_sp m_syslog_sink_sp;
#endif

   SL m_file_sl;
   std::string m_file_channels;     // channel filter
   SL m_console_sl;
   std::string m_console_channels;  // channel filter

//#if defined(BOOST_WINDOWS)
   SL m_debugger_sl;
   std::string m_debugger_channels; // channel filter
   volatile bool m_only_within_debugger;
//#endif

   SL m_syslog_sl;
   std::string m_syslog_channels; // channel filter

   // settings
   bool m_use_utc_timestamp;

   // file logger
   bool m_file_auto_flush;
   boost::filesystem::path m_path_log_file_pattern;
   uintmax_t m_rotation_file_size_limit;     // size limit of single log file
   uintmax_t m_rotation_storage_max_used;       // limit of used
   uintmax_t m_rotation_storage_min_free;  // limit of left free space
   boost::shared_ptr<boost::log::sinks::file::collector> m_file_collector_sp;

   // syslog logger, also support by windows
#ifndef LOGGER_NO_SYSLOG
   boost::log::sinks::syslog::facility m_syslog_facility;     // default: boost::log::sinks::syslog::user;
   std::string m_syslog_target_ip;     // default is 127.0.0.1
   unsigned short m_syslog_target_port;
   std::string m_syslog_local_ip;      // default is empty (not called)
   unsigned short m_syslog_local_port;
#endif

protected:
   keeper_impl();
   virtual ~keeper_impl();

   void add_all_loggers();
   void flush();
   void destroy();
   void set_utc_timestamp(bool bUTC)     // default is local time
   {
      m_use_utc_timestamp = bUTC;
      _do_set_utc_timestamp();
   }

   //
   // File logger
   //
   void add_file_logger();
   void remove_file_logger();
   sink_file_sp get_file_sink()
   {
      return m_file_sink_sp;
   }
   bool file_sink_available() const
   {
      return (bool)m_file_sink_sp;
   }

   void set_file_pattern(boost::filesystem::path const & pattern)
   {
      m_path_log_file_pattern = boost::filesystem::absolute(pattern);
      _do_set_file_pattern();
   }

   void get_parent_path(boost::filesystem::path &path) const
   {
      path = m_path_log_file_pattern.parent_path();
   }

   void set_file_rotation_size(uintmax_t max_size)    // size limit of current log file
   {
      m_rotation_file_size_limit = max_size;
      _do_set_file_rotation_size();
   }

   // min_free = 0: free space not checked
   void set_file_storage_size(uintmax_t max_used, uintmax_t min_free)
   {
      m_rotation_storage_max_used = max_used;
      m_rotation_storage_min_free = min_free;
      _do_set_file_storage_size();
   }

   void set_file_auto_flush(bool flush)
   {
      m_file_auto_flush = flush;
      _do_set_file_auto_flush();
   }

   //
   // file logger filter setting
   //
   void set_file_severity_level(SL eSL)
   {
      if (eSL < trace || eSL > never)
      {
         throw std::invalid_argument("invalid severity level");
      }
      m_file_sl = eSL;
   }

   SL get_file_severity_level() const { return m_file_sl; }

   void set_file_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
   {
      m_file_channels = str == NULL ? "" : str;
   }

   const std::string &get_file_channels() const
   {
      return m_file_channels;
   }


   //
   // Console logger
   //

   void add_console_logger();
   void remove_console_logger();
   sink_console_sp &get_console_sink() { return m_console_sink_sp; }
   bool console_sink_available() const
   {
      return (bool)m_console_sink_sp;
   }

   //
   // console logger filter setting
   //
   void set_console_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
   {
      m_console_channels = str == NULL ? "" : str;
   }
   const std::string &get_console_channels() const
   {
      return m_console_channels;
   }

   void set_console_severity_level(SL eSL)
   {
      if (eSL < trace || eSL > never)
      {
         throw std::invalid_argument("invalid severity level");
      }
      m_console_sl = eSL;
   }
   SL get_console_severity_level() const
   {
      return m_console_sl;
   }

   //
   // Debugger logger
   //
   void add_debugger_logger();
   void remove_debugger_logger();

#if defined(BOOST_WINDOWS)
   sink_debugger_sp &get_debugger_sink() { return m_debugger_sink_sp; }
   bool debugger_sink_available() const
   {
      return (bool)m_debugger_sink_sp;
   }
#else
   bool debugger_sink_available() const
   {
      return false;
   }
#endif

   //
   // debugger logger filter setting
   //
   void set_debugger_severity_level(SL eSL)
   {
      if (eSL < trace || eSL > never)
      {
         throw std::invalid_argument("invalid severity level");
      }
      m_debugger_sl = eSL;
   }

   SL get_debugger_severity_level() const
   {
      return m_debugger_sl;
   }
   void set_debugger_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
   {
      m_debugger_channels = str == NULL ? "" : str;
   }
   const std::string &get_debugger_channels() const
   {
      return m_debugger_channels;
   }
   void set_debugger_only_in_debugger(bool only_in_debugger)
   {
      m_only_within_debugger = only_in_debugger;
   }
   bool get_debugger_only_in_debugger() const
   {
      return m_only_within_debugger;
   }

   //
   // Syslog logger
   //
   void add_syslog_logger();
   void remove_syslog_logger();

#if !defined(BOOST_WINDOWS)
   sink_syslog_sp &get_syslog_sink() { return m_syslog_sink_sp; }
   bool syslog_sink_available() const
   {
      return (bool)m_syslog_sink_sp;
   }
#else
   bool syslog_sink_available() const
   {
      return false;
   }
#endif

   void set_syslog_facility(const char *facility)
   {
      if (facility == NULL) facility = "user";
      m_syslog_facility = syslog_string_to_facility(facility);
   }

   void set_syslog_target_address(std::string const &host, unsigned short port)
   {
      m_syslog_target_ip = host;
      m_syslog_target_port = port;
      _do_set_syslog_target_address();
   }

   void set_syslog_local_address(std::string const &host, unsigned short port)
   {
      m_syslog_local_ip = host;
      m_syslog_local_port = port;
      _do_set_syslog_local_address();
   }

   //
   // syslog logger filter setting
   //
   void set_syslog_severity_level(SL eSL)
   {
      if (eSL < trace || eSL > never)
      {
         throw std::invalid_argument("invalid severity level");
      }
      m_syslog_sl = eSL;
   }

   SL get_syslog_severity_level() const
   {
      return m_syslog_sl;
   }
   void set_syslog_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
   {
      m_syslog_channels = str == NULL ? "" : str;
   }
   const std::string &get_syslog_channels() const
   {
      return m_syslog_channels;
   }

protected:

   // all loggers
   void _do_set_utc_timestamp()
   {
      // change local clock to UTC clock
      boost::log::attribute_set set = m_log_core_sp->get_global_attributes();
      boost::log::attribute_set::iterator it = set.find(boost::log::aux::default_attribute_names::timestamp());
      if (it != set.end())
      {
         set.erase(it);
      }
      if (m_use_utc_timestamp)
      {
         set.insert(boost::log::aux::default_attribute_names::timestamp(), boost::log::attributes::utc_clock());
      } else
      {
         set.insert(boost::log::aux::default_attribute_names::timestamp(), boost::log::attributes::local_clock());
      }
      m_log_core_sp->set_global_attributes(set);
   }

   //
   // file logger specific
   //
   void _do_set_file_pattern()
   {
      sink_file_sp sink_sp = m_file_sink_sp;
      if (sink_sp)
      {
         sink_sp->locked_backend()->set_file_name_pattern(m_path_log_file_pattern);
      }
   }

   void _do_set_file_rotation_size()
   {
      sink_file_sp sink_sp = m_file_sink_sp;
      if (sink_sp)
      {
         sink_sp->locked_backend()->set_rotation_size(m_rotation_file_size_limit ? m_rotation_file_size_limit : (std::numeric_limits<uintmax_t>::max)());
      }
   }

   void _do_set_file_storage_size()
   {
      sink_file_sp sink_sp = m_file_sink_sp;
      if (sink_sp)
      {
         auto backend = sink_sp->locked_backend();
         boost::filesystem::path folder=m_path_log_file_pattern.parent_path();

         boost::shared_ptr<sinks::file::collector> collector_sp =
            sinks::file::make_collector(
            keywords::target = folder,
            keywords::max_size = m_rotation_storage_max_used ? m_rotation_storage_max_used : (std::numeric_limits<uintmax_t>::max)(),
            keywords::min_free_space = m_rotation_storage_min_free);

         backend->set_file_collector(collector_sp);
         backend->scan_for_files(sinks::file::scan_matching);     // matching template and update counter
      }
   }

   void _do_set_file_auto_flush()
   {
      sink_file_sp sink_sp = m_file_sink_sp;
      if (sink_sp)
      {
         sink_sp->locked_backend()->auto_flush(m_file_auto_flush);
      }
   }

   //
   // syslog specific
   //
   void _do_set_syslog_target_address()
   {
#ifndef LOGGER_NO_SYSLOG
      sink_syslog_sp sink_sp = m_syslog_sink_sp;
      if (sink_sp)
      {
         sink_sp->locked_backend()->set_target_address(m_syslog_target_ip, m_syslog_target_port);
      }
#endif
   }

   void _do_set_syslog_local_address()
   {
#ifndef LOGGER_NO_SYSLOG
      sink_syslog_sp sink_sp = m_syslog_sink_sp;
      if (sink_sp)
      {
         sink_sp->locked_backend()->set_local_address(m_syslog_local_ip, m_syslog_local_port);
      }
#endif
   }

public:

static bool log_filter_file(keeper_impl *pThis, logging::value_ref<SL, tag::severity> const &level, logging::value_ref<std::string, tag::channel> const &channel);
static bool log_filter_console(keeper_impl *pThis, boost::log::value_ref<SL, tag::severity> const &level, boost::log::value_ref<std::string, tag::channel> const &channel);

#if defined(BOOST_WINDOWS)
static bool log_filter_debugger(keeper_impl *pThis, boost::log::value_ref<SL, tag::severity> const &level, boost::log::value_ref<std::string, tag::channel> const &channel, bool bInDebugger);
#endif

#ifndef LOGGER_NO_SYSLOG
static bool log_filter_syslog(keeper_impl *pThis, boost::log::value_ref<SL, tag::severity> const &level, boost::log::value_ref<std::string, tag::channel> const &channel);
#endif
};

bool keeper_impl::log_filter_file(keeper_impl *pThis, logging::value_ref<SL, tag::severity> const &level, logging::value_ref<std::string, tag::channel> const &channel)
{
   if (level < pThis->m_file_sl)
      return false;
   if (!pThis->m_file_channels.empty() && channel)
   {
      return boost::ifind_first(pThis->m_file_channels, channel.get<std::string>());
   }
   return true;
}

bool keeper_impl::log_filter_console(keeper_impl *pThis, logging::value_ref<SL, tag::severity> const &level, logging::value_ref<std::string, tag::channel> const &channel)
{
   if (level < pThis->m_console_sl)
      return false;
   if (!pThis->m_console_channels.empty() && channel)
   {
      return boost::ifind_first(pThis->m_console_channels, channel.get<std::string>());
   }
   return true;
}

#if defined(BOOST_WINDOWS)
bool keeper_impl::log_filter_debugger(keeper_impl *pThis, logging::value_ref<SL, tag::severity> const &level, logging::value_ref<std::string, tag::channel> const &channel, bool in_debugger)
{
   if (pThis->m_only_within_debugger && !in_debugger)
      return false;
   if (level < pThis->m_debugger_sl)
      return false;
   if (!pThis->m_debugger_channels.empty() && channel)
   {
      return boost::ifind_first(pThis->m_debugger_channels, channel.get<std::string>());
   }
   return true;
}
#endif


#ifndef LOGGER_NO_SYSLOG
bool keeper_impl::log_filter_syslog(keeper_impl *pThis, logging::value_ref<SL, tag::severity> const &level, logging::value_ref<std::string, tag::channel> const &channel)
{
   if (level < pThis->m_syslog_sl)
      return false;
   if (!pThis->m_syslog_channels.empty() && channel)
   {
      return boost::ifind_first(pThis->m_syslog_channels, channel.get<std::string>());
   }
   return true;
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
// exception handler
//
// http://www.boost.org/doc/libs/1_58_0/libs/log/doc/html/log/detailed/utilities.html
/////////////////////////////////////////////////////////////////////////////
struct log_exception_handler
{
   typedef void result_type;
   void operator() (std::runtime_error const & e) const
   {
      std::cerr << "logger exception: std::runtime_error - " << e.what() << std::endl;
   }
   void operator() (std::logic_error const & e) const
   {
      std::cerr << "logger exception: std::logic_error - " << e.what() << std::endl;
      throw;
   }
   void operator() (std::exception const &e) const
   {
      std::cerr << "logger exception: std::exception - " << e.what() << std::endl;
   }
   void operator() () const
   {
      std::cerr << "logger exception: unknown exception type" << std::endl;
   }
};

/////////////////////////////////////////////////////////////////////////////
//
// keeper_impl
//
/////////////////////////////////////////////////////////////////////////////

keeper_impl::keeper_impl()
   : m_log_core_sp(logging::core::get())
   , m_file_sl(error)
   , m_console_sl(fatal)
   , m_debugger_sl(trace)
   , m_syslog_sl(trace)
   , m_use_utc_timestamp(LOGGER_KEEPER_DEFAULT_USE_UTC_TIMESTAMP)
   , m_path_log_file_pattern(LOGGER_KEEPER_DEFAULT_LOG_FILE_PATTERN)
   , m_rotation_file_size_limit(LOGGER_KEEPER_DEFAULT_LOG_FILE_ROTATION_SIZE)
   , m_rotation_storage_max_used(LOGGER_KEEPER_DEFAULT_LOG_FILE_STORAGE_MAX_USED)
   , m_rotation_storage_min_free(LOGGER_KEEPER_DEFAULT_LOG_FILE_STORAGE_MIN_SPACE)
   , m_file_auto_flush(LOGGER_KEEPER_DEFAULT_LOG_FILE_ENABLE_AUTO_FLUSH)
   , m_only_within_debugger(LOGGER_KEEPER_DEFAULT_IN_DEBUGGER_ONLY)
#ifndef LOGGER_NO_SYSLOG
   , m_syslog_facility(boost::log::sinks::syslog::user)
   , m_syslog_target_ip("127.0.0.1")
   , m_syslog_target_port(514)
   // local ip is empty
   , m_syslog_local_port(514)
#endif
{
   // #include <boost/log/utility/exception_handler.hpp>
   m_log_core_sp->set_exception_handler(boost::log::make_exception_handler<std::runtime_error,std::logic_error>(log_exception_handler(), std::nothrow));

   // common: line_id, timestamp, process_id, thread_id
   //    pCore->add_global_attribute(
   //       aux::default_attribute_names::timestamp(),
   //       attributes::local_clock());

   logging::add_common_attributes();
}

keeper_impl::~keeper_impl()
{
   destroy();
}

void keeper_impl::add_file_logger()
{
   if (m_file_sink_sp)
   {
      return;
   }

   //
   // W3C date time format: 1997-07-16T19:20:30.45+01:00
   //
   // boost::posix_time::ptime does not provide UTC conversion or TZ info

   m_file_sink_sp = logging::add_file_log
      (
      keywords::file_name = LOGGER_KEEPER_DEFAULT_LOG_FILE_PATTERN,
      keywords::rotation_size = LOGGER_KEEPER_DEFAULT_LOG_FILE_ROTATION_SIZE,
      keywords::open_mode = std::ios_base::app | std::ios_base::out,
      keywords::time_based_rotation = ::boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::format =
      (
         expr::stream
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
            << ": <" << severity << "> "
            << expr::if_ (expr::has_attr<std::string>("Channel"))
            [
               expr::stream << "[" << channel << "] "
            ]
            << expr::message
         )
      );
   // the logging file will be in UTF-8
   m_file_sink_sp->imbue(boost::locale::generator()("en_US.UTF-8"));

   // m_file_sink_sp->set_filter(!expr::has_attr(severity) || severity >= SL::error);
   m_file_sink_sp->set_filter(boost::phoenix::bind(&keeper_impl::log_filter_file, this, severity.or_none(), channel.or_none()));
   // m_file_sink_sp->set_formatter(&log_formatter_file);

   _do_set_file_pattern();
   _do_set_file_auto_flush();
   _do_set_file_rotation_size();
   _do_set_file_storage_size();
}

void keeper_impl::remove_file_logger()
{
   if (m_file_sink_sp)
   {
      m_log_core_sp->remove_sink(m_file_sink_sp);
      m_file_sink_sp.reset();
   }
   m_file_collector_sp.reset();
}

void keeper_impl::add_console_logger()
{
   if (m_console_sink_sp)
   {
      return;
   }
   m_console_sink_sp = logging::add_console_log
   (
      std::wclog,
      keywords::format =
      (
         expr::stream
         << L"<" << severity << L"> "
         << expr::if_ (expr::has_attr<std::string>("Channel"))
         [
            expr::stream << L"[" << channel << L"] "
         ]
         << expr::message
      )
   );
   m_console_sink_sp->set_filter(boost::phoenix::bind(&keeper_impl::log_filter_console, this, severity.or_none(), channel.or_none()));
   // m_console_sink_sp->set_formatter(&log_formatter_console);
}

void keeper_impl::add_debugger_logger()
{
   // not suppose to call imbue here, because clog,cerr,cout should be configure by application itself (main())
#if defined(BOOST_WINDOWS)
   if (m_debugger_sink_sp)
   {
      return;
   }
   m_debugger_sink_sp.reset(new ::boost::log::sinks::synchronous_sink< boost::log::sinks::wdebug_output_backend>());
   m_debugger_sink_sp->set_filter(boost::phoenix::bind(&keeper_impl::log_filter_debugger, this, severity.or_none(), channel.or_none(), expr::is_debugger_present()));
   m_debugger_sink_sp->set_formatter
      (
         expr::stream
         << L"<" << severity << L"> "
         << expr::if_ (expr::has_attr<std::string>("Channel"))
         [
            expr::stream << "[" << channel << "] "
         ]
         << expr::message << L"\r\n"
      );
   m_log_core_sp->add_sink(m_debugger_sink_sp);
#endif
}

#ifndef LOGGER_NO_SYSLOG

static sinks::syslog::facility syslog_string_to_facility(const char *facility)
{
   std::string str = facility;
   boost::algorithm::to_lower(str);
   if (str == "kernel") return sinks::syslog::kernel;
   if (str == "user") return sinks::syslog::user;
   if (str == "mail") return sinks::syslog::mail;
   if (str == "daemon") return sinks::syslog::daemon;
   if (str == "security0") return sinks::syslog::security0;
   if (str == "syslogd") return sinks::syslog::syslogd;
   if (str == "printer") return sinks::syslog::printer;
   if (str == "news") return sinks::syslog::news;
   if (str == "uucp") return sinks::syslog::uucp;
   if (str == "clock0") return sinks::syslog::clock0;
   if (str == "security1") return sinks::syslog::security1;
   if (str == "ftp") return sinks::syslog::ftp;
   if (str == "ntp") return sinks::syslog::ntp;
   if (str == "log_audit") return sinks::syslog::log_audit;
   if (str == "log_alert") return sinks::syslog::log_alert;
   if (str == "clock1") return sinks::syslog::clock1;
   if (str == "local0") return sinks::syslog::local0;
   if (str == "local1") return sinks::syslog::local1;
   if (str == "local2") return sinks::syslog::local2;
   if (str == "local3") return sinks::syslog::local3;
   if (str == "local4") return sinks::syslog::local4;
   if (str == "local5") return sinks::syslog::local5;
   if (str == "local6") return sinks::syslog::local6;
   if (str == "local7") return sinks::syslog::local7;
   return sinks::syslog::user;
}

#endif

void keeper_impl::add_syslog_logger()
{
   // not suppose to call imbue here, because clog,cerr,cout should be configure by application itself (main())
#ifndef LOGGER_NO_SYSLOG
   if (m_syslog_sink_sp)
   {
      return;
   }

   m_syslog_sink_sp.reset(new sinks::synchronous_sink< boost::log::sinks::syslog_backend>(
      keywords::facility = m_syslog_facility,
      keywords::use_impl = sinks::syslog::udp_socket_based
      ));
   // Create and fill in another level translator for "Severity" attribute of type string
   {
      sinks::syslog::custom_severity_mapping< std::string > mapping("Severity");
      mapping["trace"] = sinks::syslog::debug;
      mapping["debug"] = sinks::syslog::debug;
      mapping["normal"] = sinks::syslog::info;
      mapping["warning"] = sinks::syslog::warning;
      mapping["warn"] = sinks::syslog::warning;
      mapping["error"] = sinks::syslog::error;
      mapping["fatal"] = sinks::syslog::critical;
      mapping["never"] = sinks::syslog::emergency;
      mapping["always"] = sinks::syslog::emergency;

      m_syslog_sink_sp->locked_backend()->set_severity_mapper(mapping);

      _do_set_syslog_target_address();
      if (!m_syslog_local_ip.empty())
      {
         _do_set_syslog_local_address();
      }
   }
   m_syslog_sink_sp->set_filter(boost::phoenix::bind(&keeper_impl::log_filter_syslog, this, severity.or_none(), channel.or_none()));
   m_syslog_sink_sp->set_formatter
      (
         expr::stream
            << "<" << severity << "> "
            << expr::if_ (expr::has_attr<std::string>("Channel"))
            [
               expr::stream << "[" << channel << "] "
            ]
            << expr::message
      );
   m_log_core_sp->add_sink(m_syslog_sink_sp);
#endif
}

void keeper_impl::remove_console_logger()
{
   if (m_console_sink_sp)
   {
      m_log_core_sp->remove_sink(m_console_sink_sp);
      m_console_sink_sp.reset();
   }
}

void keeper_impl::remove_debugger_logger()
{
#if defined(BOOST_WINDOWS)
   if (m_debugger_sink_sp)
   {
      m_log_core_sp->remove_sink(m_debugger_sink_sp);
      m_debugger_sink_sp.reset();
   }
#endif
}

void keeper_impl::remove_syslog_logger()
{
#if !defined(LOGGER_NO_SYSLOG)
   if (m_syslog_sink_sp)
   {
      m_log_core_sp->remove_sink(m_syslog_sink_sp);
      m_syslog_sink_sp.reset();
   }
#endif
}

void keeper_impl::add_all_loggers()
{
   add_file_logger();
   add_console_logger();
   add_debugger_logger();
   add_syslog_logger();
}

void keeper_impl::flush()
{
   if (m_log_core_sp)
   {
      m_log_core_sp->flush();
   }
}

void keeper_impl::destroy()
{
   if (m_log_core_sp)
   {
      m_log_core_sp->set_logging_enabled(false);
      m_log_core_sp->flush();
   }

#if !defined(BOOST_WINDOWS)
   remove_syslog_logger();
#endif

#if defined(BOOST_WINDOWS)
   remove_debugger_logger();
#endif
   remove_console_logger();
   remove_file_logger();

   if (m_log_core_sp)
   {
      boost::log::attribute_set set;
      m_log_core_sp->set_global_attributes(set);    // set empty set

      m_log_core_sp->remove_all_sinks();
      m_log_core_sp.reset();
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// logger_keeper
//
/////////////////////////////////////////////////////////////////////////////


// Class Template Instantiation

// static/specialization, need template<>

//template<>
//boost::atomic<gorilla::log::logger_keeper *> gorilla::log::t_singleton<gorilla::log::logger_keeper>::s_pInstance(NULL);
//template<>
//boost::mutex gorilla::log::t_singleton<gorilla::log::logger_keeper>::s_mutexInstantiation;

boost::atomic<gorilla::log::logger_keeper *> gorilla::log::logger_keeper::s_pInstance(NULL);
boost::mutex gorilla::log::logger_keeper::s_mutexInstantiation;


// static
#ifdef BOOST_WINDOWS
// static
void __cdecl logger_keeper::destroy_at_exit(void)
#else
// static
void logger_keeper::destroy_at_exit(void)
#endif
{
   logger_keeper *ptr = s_pInstance.load();
   delete ptr;
   s_pInstance.store(NULL);
}

// singleton implementation
// static
logger_keeper* logger_keeper::get_instance()
{
   logger_keeper *ptr = s_pInstance.load();
   if (ptr == NULL)
   {
      boost::mutex::scoped_lock lock(s_mutexInstantiation);
      // again, after locked
      ptr = s_pInstance.load();
      if (ptr == NULL)
      {
         ptr = new logger_keeper();
         s_pInstance.store(ptr);
         atexit(&destroy_at_exit);
      }
   }
   return ptr;
}

// static
void logger_keeper::destroy_instance()
{
   logger_keeper *ptr = s_pInstance.load();
   delete ptr;
   s_pInstance.store(NULL);
}

logger_keeper::logger_keeper()
   : m_impl(new keeper_impl())
{
}

logger_keeper::~logger_keeper()
{
   delete m_impl;
   m_impl = NULL;
}

void logger_keeper::add_all_loggers()
{
   m_impl->add_all_loggers();
}

void logger_keeper::flush()
{
   m_impl->flush();
}

void logger_keeper::destroy()
{
   m_impl->destroy();
}

void logger_keeper::set_utc_timestamp(bool bUTC)     // default is local time
{
   m_impl->set_utc_timestamp(bUTC);
}

//
// file logger
//
void logger_keeper::add_file_logger()
{
   m_impl->add_file_logger();
}

void logger_keeper::remove_file_logger()
{
   m_impl->remove_file_logger();
}

bool logger_keeper::is_file_logger_available() const
{
   return m_impl->file_sink_available();
}

void logger_keeper::set_file_severity_level(SL eSL)
{
   m_impl->set_file_severity_level(eSL);
}

SL logger_keeper::get_file_severity_level() const
{
   return m_impl->get_file_severity_level();
}

void logger_keeper::set_file_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
{
   m_impl->set_file_channels(str);
}

const std::string &logger_keeper::get_file_channels() const
{
   return m_impl->get_file_channels();
}

sink_file_sp logger_keeper::get_file_sink() const
{
   return m_impl->get_file_sink();
}

void logger_keeper::set_file_pattern(const char *pattern)
{
   m_impl->set_file_pattern(boost::filesystem::path(pattern));
}

template< typename PathT > void logger_keeper::set_file_pattern(PathT const & pattern)
{
   m_impl->set_file_pattern(pattern);
}

void logger_keeper::get_pattern_parent_path(boost::filesystem::path &path) const
{
   m_impl->get_parent_path(path);
}

void logger_keeper::set_file_rotation_size(uintmax_t max_size)    // size limit of current log file
{
   m_impl->set_file_rotation_size(max_size);
}

void logger_keeper::set_file_storage_size(uintmax_t max_used, uintmax_t min_free)
{
   m_impl->set_file_storage_size(max_used, min_free);
}

void logger_keeper::set_file_auto_flush(bool flush)
{
   m_impl->set_file_auto_flush(flush);
}

//
// console logger
//
void logger_keeper::add_console_logger()
{
   m_impl->add_console_logger();
}

void logger_keeper::remove_console_logger()
{
   m_impl->remove_console_logger();
}

bool logger_keeper::is_console_logger_available() const
{
   return m_impl->console_sink_available();
}
//
// console logger filter setting
//
void logger_keeper::set_console_severity_level(SL eSL)
{
   m_impl->set_console_severity_level(eSL);
}

SL logger_keeper::get_console_severity_level() const
{
   return m_impl->get_console_severity_level();
}

void logger_keeper::set_console_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
{
   m_impl->set_console_channels(str);
}

const std::string &logger_keeper::get_console_channels() const
{
   return m_impl->get_console_channels();
}

sink_console_sp &logger_keeper::get_console_sink() const
{
   return m_impl->get_console_sink();
}
//
// debugger logger
//
void logger_keeper::add_debugger_logger()
{
   m_impl->add_debugger_logger();
}

void logger_keeper::remove_debugger_logger()
{
   m_impl->remove_debugger_logger();
}

bool logger_keeper::is_debugger_logger_available() const
{
   return m_impl->debugger_sink_available();
}

void logger_keeper::set_debugger_severity_level(SL eSL)
{
   m_impl->set_debugger_severity_level(eSL);
}

SL logger_keeper::get_debugger_severity_level() const
{
   return m_impl->get_debugger_severity_level();
}

void logger_keeper::set_debugger_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
{
   m_impl->set_debugger_channels(str);
}

const std::string &logger_keeper::get_debugger_channels() const
{
   return m_impl->get_debugger_channels();
}

void logger_keeper::set_debugger_only_in_debugger(bool only_in_debugger)
{
   m_impl->set_debugger_only_in_debugger(only_in_debugger);
}

bool logger_keeper::get_debugger_only_in_debugger() const
{
   return m_impl->get_debugger_only_in_debugger();
}

#if defined(BOOST_WINDOWS)
sink_debugger_sp &logger_keeper::get_debugger_sink() const
{
   return m_impl->get_debugger_sink();
}
#endif

//
// syslog logger
//
void logger_keeper::add_syslog_logger()
{
   m_impl->add_syslog_logger();
}

void logger_keeper::remove_syslog_logger()
{
   m_impl->remove_syslog_logger();
}

bool logger_keeper::is_syslog_logger_available() const
{
   return m_impl->syslog_sink_available();
}

void logger_keeper::set_syslog_facility(const char *facility)
{
   m_impl->set_syslog_facility(facility);
}

void logger_keeper::set_syslog_target_address(const char *host, unsigned short port)
{
   std::string str_ip = (host == NULL) ? "127.0.0.1" : host;
   if (port == 0) port = 514;
   m_impl->set_syslog_target_address(str_ip, port);
}

void logger_keeper::set_syslog_local_address(const char *host, unsigned short port)
{
   std::string str_ip = (host == NULL) ? "127.0.0.1" : host;
   if (port == 0) port = 514;
   m_impl->set_syslog_local_address(str_ip, port);
}

void logger_keeper::set_syslog_severity_level(SL eSL)
{
   m_impl->set_syslog_severity_level(eSL);
}

SL logger_keeper::get_syslog_severity_level() const
{
   return m_impl->get_syslog_severity_level();
}

void logger_keeper::set_syslog_channels(const char *str)     // set channel filters (a list of channels separate by comma or space )
{
   m_impl->set_syslog_channels(str);
}

const std::string &logger_keeper::get_syslog_channels() const
{
   return m_impl->get_syslog_channels();
}


#if !defined(BOOST_WINDOWS)
sink_syslog_sp &logger_keeper::get_syslog_sink() const
{
   return m_impl->get_syslog_sink();
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Interface functions
//
/////////////////////////////////////////////////////////////////////////////


void logger_init()
{
   logger_keeper::get_instance();         //
}

void logger_flush()
{
   logger_keeper::get_instance()->flush();      // default will add file logger
}

void logger_destroy()
{
   logger_keeper::get_instance()->destroy();      // default will add file logger
   logger_keeper::destroy_instance();
}



   } // log
} // gorilla

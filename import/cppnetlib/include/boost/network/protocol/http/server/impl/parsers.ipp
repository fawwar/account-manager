#ifndef SERVER_REQUEST_PARSERS_IMPL_UW3PM6V6
#define SERVER_REQUEST_PARSERS_IMPL_UW3PM6V6

#define BOOST_SPIRIT_UNICODE
#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/qi_parse.hpp>
//#include <boost/spirit/include/support_standard_wide.hpp>

// Copyright 2013 Google, Inc.
// Copyright 2010 Dean Michael Berris.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/network/protocol/http/message/header.hpp>
#include <boost/fusion/tuple.hpp>

#ifdef BOOST_NETWORK_NO_LIB
#ifndef BOOST_NETWORK_INLINE
#define BOOST_NETWORK_INLINE inline
#endif
#else
#define BOOST_NETWORK_INLINE
#endif
#include <vector>

namespace boost {
namespace spirit {
namespace traits {

typedef std::basic_string<uint32_t> u32_string;

template <>  // <typename Attrib, typename T, typename Enable>
struct assign_to_container_from_value<std::string, u32_string, void> {
  static void call(u32_string const& val, std::string& attr) {
    u32_to_u8_iterator<u32_string::const_iterator> begin = val.begin(),
                                                   end = val.end();
    for (; begin != end; ++begin) attr += *begin;
  }
};

}  // namespace traits
}  // namespace spirit
}  // namespace boost

namespace boost {
namespace network {
namespace http {
//ENRICO: FIX VC2017 2017.12.29
BOOST_NETWORK_INLINE void parse_version(
    std::string const& partial_parsed,
    fusion::tuple<uint8_t, uint8_t>& version_pair) {
  boost::spirit::qi::parse(partial_parsed.begin(), partial_parsed.end(),
        (boost::spirit::qi::lit("HTTP/") >> boost::spirit::qi::ushort_ >> '.' >> boost::spirit::qi::ushort_), version_pair);
}

BOOST_NETWORK_INLINE void parse_headers(
    std::string const& input, std::vector<request_header_narrow>& container) {
  u8_to_u32_iterator<std::string::const_iterator> begin = input.begin(),
                                                  end = input.end();
  typedef boost::spirit::qi::as<boost::spirit::traits::u32_string> as_u32_string;
  boost::spirit::qi::parse(begin, end,
        *(+((boost::spirit::qi::alnum | boost::spirit::qi::punct) - ':') >> boost::spirit::qi::lit(": ") >>
          as_u32_string()[+((boost::spirit::qi::alnum | boost::spirit::qi::space | boost::spirit::qi::punct) - '\r' - '\n')] >>
          boost::spirit::qi::lit("\r\n")) >>
            boost::spirit::qi::lit("\r\n"),
        container);
}

}  // namespace http
}  // namespace network
}  // namespace boost

#endif /* SERVER_REQUEST_PARSERS_IMPL_UW3PM6V6 */

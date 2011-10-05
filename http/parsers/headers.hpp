//
// headers.hpp
// ~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_PARSERS_HEADERS_HPP
#define HTTP_PARSERS_HEADERS_HPP

#include <http/headers.hpp>
#include <http/parsers/date.hpp>
#include <http/parsers/media_type.hpp>
#include <http/parsers/basic_rules.hpp>

#include <boost/spirit/include/qi_parse_auto.hpp>
#include <boost/spirit/home/qi/numeric.hpp>
#include <boost/spirit/home/qi/operator.hpp>

namespace http { namespace parsers {

template <typename Header, typename InputIterator>
bool parse_header(Header& header, InputIterator begin, InputIterator end)
{
    header.second = std::string(begin, end);
    return true;
}

template <typename InputIterator>
bool parse_header(headers::cache_control& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> -('=' >> (b.token | b.quoted_string))) % ',', b.skipper, header.second);
}

template <typename InputIterator>
bool parse_header(headers::connection& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, header.second);
}

template <typename InputIterator>
bool parse_header(headers::date& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, date_grammar<InputIterator>(), b.skipper, header.second);;
}

template <typename InputIterator>
bool parse_header(headers::pragma& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> -('=' >> (b.token | b.quoted_string))) % ',', b.skipper, header.second);
}

template <typename InputIterator>
bool parse_header(headers::trailer& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, header.second);
}

template <typename InputIterator>
bool parse_header(headers::allow& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, header.second);
}

template <typename InputIterator>
bool parse_header(headers::content_length& header, InputIterator begin, InputIterator end)
{
    return boost::spirit::qi::parse(begin, end, header.second);
}

template <typename InputIterator>
bool parse_header(headers::content_type& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, media_type<InputIterator>(), b.skipper, header.second);
}

/*
template <typename InputIterator>
bool parse_header(headers::& header, InputIterator begin, InputIterator end)
{
}
*/

} } // namespace parsers namespace http

#endif

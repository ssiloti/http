//
// response.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_PARSERS_RESPONSE_HPP
#define HTTP_PARSERS_RESPONSE_HPP

#include <http/parsers/basic_rules.hpp>
#include <http/parsers/version_number.hpp>

#include <boost/spirit/home/qi/numeric/uint.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>
#include <boost/spirit/home/qi/parse.hpp>

namespace http {

template <typename Headers, typename Body>
template <typename Iterator>
struct basic_response<Headers, Body>::status_line_grammar
    : boost::spirit::qi::grammar<Iterator, tuple_type()>
{
    status_line_grammar() : status_line_grammar::base_type(status_line)
    {
        using namespace boost::spirit;
        status_line %= version_number_g >> qi::omit[b.sp]
                    >> qi::uint_parser<unsigned, 10, 3, 3>() >> qi::omit[b.sp]
                    >> reason_phrase;
        reason_phrase %= *( b.wsp | b.vchar | b.obs_text );
    }

    parsers::basic_rules<Iterator> b;
    parsers::version_number_grammar<Iterator> version_number_g;
    boost::spirit::qi::rule<Iterator, std::string()> reason_phrase;
    boost::spirit::qi::rule<Iterator, tuple_type()> status_line;
};

template <typename Headers, typename Body>
template <typename InputIterator>
bool basic_response<Headers, Body>::parse_start_line(InputIterator begin, InputIterator end)
{
    return boost::spirit::qi::parse(
        begin,
        end,
        status_line_grammar<InputIterator>(),
        tuple_type(version, status, reason)
    );
}

}

#endif

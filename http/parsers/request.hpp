//
// request.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_PARSERS_REQUEST_HPP
#define HTTP_PARSERS_REQUEST_HPP

#include <http/parsers/basic_rules.hpp>
#include <http/parsers/version_number.hpp>

#include <uri/components.hpp>
#include <uri/absolute_uri_grammar.hpp>

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>

namespace http {

template <typename Headers, typename Body>
template <typename Iterator>
struct basic_request<Headers, Body>::request_line_grammar
    : boost::spirit::qi::grammar<Iterator, tuple_type()>
{
    request_line_grammar() : request_line_grammar::base_type(request_line)
    {
        using namespace boost::spirit;

        request_line %= method >> qi::omit[b.sp] >> request_target >> qi::omit[b.sp] >> version_number_g;
        method %= b.token;
        abs_path %= path_absolute >> -(qi::lit('?') >> query);
        request_target %= (qi::lit('*') >> qi::attr(asterisk())) | absolute_uri | abs_path | authority;
    }

    parsers::basic_rules<Iterator> b;

    uri::absolute_uri_grammar<Iterator, std::string> absolute_uri;

    uri::path_absolute_grammar<Iterator, std::string> path_absolute;
    uri::query_grammar<Iterator, std::string> query;

    uri::authority_grammar<Iterator, std::string> authority;

    parsers::version_number_grammar<Iterator> version_number_g;

    boost::spirit::qi::rule<Iterator, tuple_type()> request_line;
    boost::spirit::qi::rule<Iterator, std::string()> method;
    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<std::string, boost::optional<std::string> >()> abs_path;
    boost::spirit::qi::rule<Iterator, target_type()> request_target;
};

template <typename Headers, typename Body>
template <typename InputIterator>
bool basic_request<Headers, Body>::parse_start_line(InputIterator begin, InputIterator end)
{
    return boost::spirit::qi::parse(
        begin,
        end,
        request_line_grammar<InputIterator>(),
        tuple_type(method, target, version)
    );
}

#if 0
template <typename Headers, typename Body>
template <typename InputIterator>
bool basic_request<Headers, Body>::parse_start_line(InputIterator begin, InputIterator end)
{
    typedef basic_request<Headers, Body> request_type;
    return boost::spirit::qi::parse(
        begin,
        end,
        detail::request_line_grammar<request_type, InputIterator>(),
        typename detail::request_tuple<request_type>::type
            (method, target, version)
    );
}
#endif
} // namespace http

#endif

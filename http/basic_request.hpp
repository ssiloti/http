//
// basic_request.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_REQUEST_HPP
#define HTTP_BASIC_REQUEST_HPP

#include <http/basic_message.hpp>
#include <uri/http/uri.hpp>
#include <http/basic_parse_rules.hpp>

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/fusion/container/generation/vector_tie.hpp>
#include <boost/variant.hpp>

namespace http {

template <typename Headers, typename Body>
class basic_request : public basic_message<Headers, Body>
{
    template <typename iterator>
    static struct request_line_grammar : boost::spirit::qi::grammar<iterator>
    {
        request_line_grammar() : request_line_grammar::base_type(request_line)
        {
            const basic_parse_rules<InputIterator>& b = basic_parse_rules<InputIterator>::get();
            const uri::basic_parse_rules<InputIterator, std::string>& u = uri::basic_parse_rules<InputIterator, std::string>::get();

            request_line = method >> b.sp >> request_target >> b.sp >> version_number::parse_rule<iterator>() >> b.crlf;
            method = b.target;
            request_target = '*' | u.absolute_uri | (u.path_absolute >> -('?' >> query)) | u.authority;
        }

        boost::spirit::qi::rule<iterator> request_line;
        boost::spirit::qi::rule<iterator> method;
        boost::spirit::qi::rule<iterator> request_target;
    };

public:
    template <typename InputIterator>
    bool parse_start_line(InputIterator begin, InputIterator end)
    {
        static struct request_line_grammar : boost::spirit::qi::grammar<InputIterator>
        {

        } grammar;

        const basic_parse_rules<InputIterator>& b = basic_parse_rules<InputIterator>::get();
        boost::spirit::qi::parse(begin, end
            , b.token >> b.sp >> (uri::http_uri_grammar<InputIterator, std::string>() | boost::spirit::qi::string()) >> version_number::parse_rule<InputIterator>()
            , boost::fusion::vector_tie(method, request_target, version));
    }

    template <typename InputIterator>
    bool parse_header(InputIterator begin, InputIterator end)
    {
    }

    std::string method;
    boost::variant<uri::authority<std::string>, uri::http_uri<std::string> > request_target;
};

}

#endif

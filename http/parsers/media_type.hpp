//
// media_type.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_PARSERS_MEDIA_TYPE_HPP
#define HTTP_PARSERS_MEDIA_TYPE_HPP

#include <http/media_type.hpp>
#include <http/parsers/basic_rules.hpp>

#include <boost/spirit/home/qi/operator.hpp>
#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

namespace http { namespace parsers {


template <typename Iterator>
struct media_type
    : boost::spirit::qi::grammar<Iterator, http::media_type()>
{
    media_type() : media_type::base_type(start, "media_type")
    {
        start %= b.token >> '/' >> b.token >> parameters;
        parameters %= *(b.ows >> ';' >> b.ows >> parameter);
        parameter %= b.token >> '=' >> b.word;
    }

    basic_rules<Iterator> b;

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, http::media_type()> start;

    boost::spirit::qi::rule<Iterator, std::map<std::string, std::string>()> parameters;
    boost::spirit::qi::rule<Iterator, std::pair<std::string, std::string>()> parameter;
};

} }

#endif

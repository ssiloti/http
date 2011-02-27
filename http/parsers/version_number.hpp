//
// version_number.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_PARSERS_VERSION_NUMBER_HPP
#define HTTP_PARSERS_VERSION_NUMBER_HPP

#include <http/version_number.hpp>

#include <boost/spirit/home/qi/numeric.hpp>
#include <boost/spirit/home/qi/string.hpp>
#include <boost/spirit/home/qi/operator.hpp>
#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

namespace http { namespace parsers {

template <typename Iterator>
struct version_number_grammar
    : boost::spirit::qi::grammar<Iterator, version_number()>
{
    version_number_grammar() : version_number_grammar::base_type(start, "version_number")
    {
        using namespace boost::spirit;

        start %= qi::lit("HTTP/") >> qi::uint_ >> '.' >> qi::uint_;
    }

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, version_number()> start;
};

} } // namespace parsers namespace http

#endif

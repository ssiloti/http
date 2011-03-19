//
// version_number.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_GENERATORS_VERSION_NUMBER_HPP
#define HTTP_GENERATORS_VERSION_NUMBER_HPP

#include <http/version_number.hpp>

#include <boost/spirit/home/karma/numeric.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>

#include <boost/spirit/home/karma/nonterminal/rule.hpp>
#include <boost/spirit/home/karma/nonterminal/grammar.hpp>

namespace http { namespace generators {

template <typename Iterator>
struct version_number_grammar
    : boost::spirit::karma::grammar<Iterator, version_number()>
{
    version_number_grammar() : version_number_grammar::base_type(start, "version_number")
    {
        using namespace boost::spirit::karma;

        start %= lit("HTTP/") << uint_ << '.' << uint_;
    }

    // start rule of grammar
    boost::spirit::karma::rule<Iterator, version_number()> start;
};

} } // namespace generators namespace http

#endif

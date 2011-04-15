//
// generic_uri.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef URI_GENERATORS_GENERIC_URI_HPP
#define URI_GENERATORS_GENERIC_URI_HPP

#include <uri/basic_uri.hpp>
#include <uri/generators/authority.hpp>

#include <boost/spirit/home/karma/directive/buffer.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>
#include <boost/spirit/home/karma/nonterminal/grammar.hpp>

namespace uri { namespace generators {

template <typename Iterator, class String>
struct generic_uri : public boost::spirit::karma::grammar<Iterator, basic_uri<String>()>
{
    typedef String string_type;

    generic_uri() : generic_uri::base_type(start, "generic_uri")
    {
        using namespace boost::spirit::karma;

        uri %= string
            << "://" << -authority_
            << string
            << buffer[-('?' << string)]
            << buffer[-('#' << string)];

        start %= uri.alias();
    }

private:
    authority<Iterator, string_type> authority_;

    // start rule of grammar
    boost::spirit::karma::rule<Iterator, basic_uri<string_type>()> start;

    // actual uri parser
    boost::spirit::karma::rule<Iterator, typename detail::const_basic_uri_tuple<string_type>::type()> uri;

};

} }

#endif

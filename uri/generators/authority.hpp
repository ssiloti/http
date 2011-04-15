//
// authority.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef URI_GENERATORS_AUTHORITY_HPP
#define URI_GENERATORS_AUTHORITY_HPP

#include <uri/basic_authority.hpp>

#include <boost/spirit/home/karma/directive/buffer.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/char/char.hpp>
#include <boost/spirit/home/karma/numeric/uint.hpp>
#include <boost/spirit/home/karma/operator.hpp>
#include <boost/spirit/home/karma/nonterminal/grammar.hpp>

namespace uri { namespace generators {

template <typename Iterator, class String>
struct authority : public boost::spirit::karma::grammar<Iterator, basic_authority<String>()>
{
    typedef String string_type;

    authority() : authority::base_type(start, "authority")
    {
        using namespace boost::spirit::karma;

        authority_rule %= -(string << lit('@')) << string << buffer[-(lit(':') << uint_)];
        start %= authority_rule.alias();
    }

private:
    boost::spirit::karma::rule<Iterator, typename detail::const_authority_tuple<string_type>::type()> authority_rule;
    boost::spirit::karma::rule<Iterator, basic_authority<string_type>()> start;
};

} }

#endif

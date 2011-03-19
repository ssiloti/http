//
// characters.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_PARSERS_CHARACTERS_HPP
#define URI_PARSERS_CHARACTERS_HPP

#include <abnf/core_rules.hpp>

#include <boost/spirit/home/qi/directive/raw.hpp>
#include <boost/spirit/home/qi/directive/repeat.hpp>

namespace uri { namespace parsers {

template <typename Iterator, typename String>
struct characters : public abnf::core_rules<Iterator>
{
    typedef String string_type;

    characters()
    {
        using namespace boost::spirit;

        // gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
        gen_delims %= qi::char_(":/?#[]@");

        // sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
        sub_delims %= qi::char_("!$&'()*+,;=");

        // reserved = gen-delims / sub-delims
        reserved %= gen_delims | sub_delims;

        // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
        unreserved %= this->alpha | this->digit | qi::char_("-._~");

        // pct-encoded = "%" HEXDIG HEXDIG
        pct_encoded %= qi::char_("%") >> qi::repeat(2)[this->hexdig];

        // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
        pchar %= qi::raw[
            unreserved | pct_encoded | sub_delims | qi::char_(":@")
        ];
    }

    boost::spirit::qi::rule<Iterator, typename string_type::value_type()>
        gen_delims, sub_delims, reserved, unreserved;
    boost::spirit::qi::rule<Iterator, string_type()>
        pct_encoded, pchar;
};

} }

#endif

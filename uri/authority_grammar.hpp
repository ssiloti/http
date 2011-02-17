//
// authority_grammar.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_AUTHORITY_GRAMMAR
#define URI_AUTHORITY_GRAMMAR

#include <uri/basic_authority.hpp>
#include <uri/character_rules.hpp>

#include <boost/spirit/home/qi/numeric/uint.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

namespace uri {

template <typename Iterator, typename String>
struct authority_grammar
    : character_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, basic_authority<String>()>
{
public:
    typedef String string_type;

    authority_grammar() : authority_grammar::base_type(start, "authority")
    {
        // user_info = *( unreserved / pct-encoded / sub-delims / ":" )
        user_info %= qi::raw[
            *(unreserved | pct_encoded | sub_delims | qi::char_(":"))
        ];

        // dec-octet = DIGIT / %x31-39 DIGIT / "1" 2DIGIT / "2" %x30-34 DIGIT / "25" %x30-35
        dec_octet %=
                !(qi::lit('0') >> qi::digit)
            >>  qi::raw[
                    qi::uint_parser<boost::uint8_t, 10, 1, 3>()
                ];

        // IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
        ipv4address %= qi::raw[
            dec_octet >> qi::repeat(3)[qi::lit('.') >> dec_octet]
        ];

        // reg-name = *( unreserved / pct-encoded / sub-delims )
        reg_name %= qi::raw[
            *(unreserved | pct_encoded | sub_delims)
        ];

        // TODO, host = IP-literal / IPv4address / reg-name
        host %= ipv4address | reg_name;

        // authority = [ userinfo "@" ] host [ ":" port ]
        authority %= (-(user_info >> qi::lit('@')) >> host >> -(qi::lit(':') >> qi::ushort_));

        // It'd be nice to use attr_cast here instead of a dummy rule, but apparently it doesn't
        // play nice when the embeded parser has sub-elements which expose unused_type attributes.
        start = authority.alias();

    }

    boost::spirit::qi::rule<Iterator, string_type()>
        dec_octet, ipv4address, reg_name, host;

    boost::spirit::qi::rule<Iterator, string_type()>
        user_info;

    boost::spirit::qi::rule<Iterator, typename detail::authority_tuple<string_type>::type()> authority;

    boost::spirit::qi::rule<Iterator, basic_authority<string_type>()> start;
};

}

#endif

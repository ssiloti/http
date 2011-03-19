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

#ifndef URI_PARSERS_AUTHORITY_HPP
#define URI_PARSERS_AUTHORITY_HPP

#include <uri/basic_authority.hpp>
#include <uri/parsers/characters.hpp>

#include <boost/spirit/home/qi/numeric/uint.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

namespace uri { namespace parsers {

namespace qi = boost::spirit::qi;

template <typename Iterator, typename String>
struct authority
    : characters<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, basic_authority<String>()>
{
    typedef String string_type;

    authority() : authority::base_type(start, "authority")
    {
        // user_info = *( unreserved / pct-encoded / sub-delims / ":" )
        user_info %= qi::raw[
            *(this->unreserved | this->pct_encoded | this->sub_delims | qi::char_(":"))
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
            *(this->unreserved | this->pct_encoded | this->sub_delims)
        ];

        // TODO, host = IP-literal / IPv4address / reg-name
        host %= ipv4address | reg_name;

        // authority_rule = [ userinfo "@" ] host [ ":" port ]
        authority_rule %= (-(user_info >> qi::lit('@')) >> host >> -(qi::lit(':') >> qi::ushort_));

        start = authority_rule.alias();

    }

    boost::spirit::qi::rule<Iterator, string_type()>
        dec_octet, ipv4address, reg_name, host;

    boost::spirit::qi::rule<Iterator, string_type()>
        user_info;

private:
    boost::spirit::qi::rule<Iterator, typename detail::authority_tuple<string_type>::type()> authority_rule;

    boost::spirit::qi::rule<Iterator, basic_authority<string_type>()> start;
};

} }

#endif

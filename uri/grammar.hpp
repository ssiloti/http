#ifndef URI_GRAMMAR_HPP
#define URI_GRAMMAR_HPP

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright 2011 Steven Siloti.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <uri/basic_uri.hpp>

#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

#include <boost/spirit/home/qi/directive/repeat.hpp>
#include <boost/spirit/home/qi/directive/raw.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>

#include <boost/spirit/home/qi/operator.hpp>

#include <boost/spirit/home/qi/char/char_class.hpp>
#include <boost/spirit/home/qi/char/char.hpp>
#include <boost/spirit/home/qi/string/lit.hpp>
#include <boost/spirit/home/qi/numeric/uint.hpp>

#include <boost/spirit/home/qi/auxiliary/eps.hpp>
#include <boost/spirit/home/qi/auxiliary/attr.hpp>

namespace uri {

namespace qi = boost::spirit::qi;

template <typename Iterator, class String>
struct grammar : boost::spirit::qi::grammar<Iterator, basic_uri<String>()>
{
    typedef String string_type;

    grammar() : grammar::base_type(start, "uri")
    {
        // gen-delims = ":" / "/" / "?" / "#" / "[" / "]" / "@"
        gen_delims %= qi::char_(":/?#[]@");
        // sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
        sub_delims %= qi::char_("!$&'()*+,;=");
        // reserved = gen-delims / sub-delims
        reserved %= gen_delims | sub_delims;

        // unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
        unreserved %= qi::alnum | qi::char_("-._~");
        // pct-encoded = "%" HEXDIG HEXDIG
        pct_encoded %= qi::char_("%") >> qi::repeat(2)[qi::xdigit];

        // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
        pchar %= qi::raw[
            unreserved | pct_encoded | sub_delims | qi::char_(":@")
        ];

        // segment = *pchar
        segment %= qi::raw[*pchar];
        // segment-nz = 1*pchar
        segment_nz %= qi::raw[+pchar];
        // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
        segment_nz_nc %= qi::raw[
            +(unreserved | pct_encoded | sub_delims | qi::char_("@"))
        ];
        // path-abempty  = *( "/" segment )
        path_abempty %= qi::raw[*(qi::char_("/") >> segment)];
        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        path_absolute %= qi::raw[
                qi::char_("/")
            >>  -(segment_nz >> *(qi::char_("/") >> segment))
        ];
        // path-rootless = segment-nz *( "/" segment )
        path_rootless %= qi::raw[
            segment_nz >> *(qi::char_("/") >> segment)
        ];
        // path-empty = 0<pchar>
        path_empty %= qi::eps;

        // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        scheme %= qi::alpha >> *(qi::alnum | qi::char_("+.-"));
 
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

        // query = *( pchar / "/" / "?" )
        query %= qi::raw[*(pchar | qi::char_("/?"))];
        // fragment = *( pchar / "/" / "?" )
        fragment %= qi::raw[*(pchar | qi::char_("/?"))];

        // authority = [ userinfo "@" ] host [ ":" port ]
        authority_t %= (-(user_info >> qi::lit('@')) >> host >> -(qi::lit(':') >> qi::ushort_));

        // It'd be nice to use attr_cast here instead of a dummy rule, but apparently it doesn't
        // play nice when the embeded parser has sub-elements which expose unused_type attributes.
        authority = authority_t.alias();

        // hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
        hier_part %=
                (qi::lit("//") >> authority >> path_abempty)
            |
                (
                    qi::attr(boost::optional<basic_authority<string_type> >())
                >>  (path_absolute | path_rootless | path_empty)
                );

        uri %= //qi::attr_cast<basic_uri<string_type>, typename detail::basic_uri_tuple<string_type>::type>(
                scheme >> ':'
            >>  hier_part
            >>  -('?' >> query)
            >>  -('#' >> fragment);

        start %= uri.alias();

    }

    boost::spirit::qi::rule<Iterator, typename string_type::value_type()>
        gen_delims, sub_delims, reserved, unreserved;
    boost::spirit::qi::rule<Iterator, string_type()>
        pct_encoded, pchar;

    boost::spirit::qi::rule<Iterator, string_type()>
        segment, segment_nz, segment_nz_nc;
    boost::spirit::qi::rule<Iterator, string_type()>
        path_abempty, path_absolute, path_rootless, path_empty;

    boost::spirit::qi::rule<Iterator, string_type()>
        dec_octet, ipv4address, reg_name, host;

    boost::spirit::qi::rule<Iterator, string_type()>
        scheme, user_info, query, fragment;

    boost::spirit::qi::rule<Iterator, typename detail::authority_tuple<string_type>::type()> authority_t;
    boost::spirit::qi::rule<Iterator, basic_authority<string_type>()> authority;

    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<string_type> >&,
        string_type &
    >()> hier_part;

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, basic_uri<string_type>()> start;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename detail::basic_uri_tuple<string_type>::type()> uri;
};

} // namespace uri

#endif // ifdef URI_GRAMMAR_HPP

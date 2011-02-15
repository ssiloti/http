#ifndef URI_GENERIC_PARSE_RULES_HPP
#define URI_GENERIC_PARSE_RULES_HPP

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_raw.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/version.hpp>

#include <boost/fusion/tuple.hpp>

namespace uri {

namespace qi = boost::spirit::qi;

template <typename Iterator, typename String>
struct basic_parse_rules
{
    typedef Iterator iterator;
    typedef String string_type;

private:
    basic_parse_rules()
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

        // hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
        // authority = [ userinfo "@" ] host [ ":" port ]
        hier_part %=
                (
                    "//"
                >>  -(user_info >> '@')
                >>  host
                >>  -(':' >> qi::ushort_)
                >>  path_abempty
                )
            |
                (
                    qi::attr(boost::optional<string_type>())
                >>  qi::attr(boost::optional<string_type>())
                >>  qi::attr(boost::optional<boost::uint16_t>())
                >>  (
                        path_absolute
                    |   path_rootless
                    |   path_empty
                    )
                );
    }

public:

    static const basic_parse_rules<iterator, string_type>& get()
    {
        static basic_parse_rules<iterator, string_type> rules;
        return rules;
    }

    boost::spirit::qi::rule<iterator, typename string_type::value_type()>
        gen_delims, sub_delims, reserved, unreserved;
    boost::spirit::qi::rule<iterator, string_type()>
        pct_encoded, pchar;

    boost::spirit::qi::rule<iterator, string_type()>
        segment, segment_nz, segment_nz_nc;
    boost::spirit::qi::rule<iterator, string_type()>
        path_abempty, path_absolute, path_rootless, path_empty;

    boost::spirit::qi::rule<iterator, string_type()>
        dec_octet, ipv4address, reg_name, host;

    boost::spirit::qi::rule<iterator, string_type()>
        scheme, user_info, query, fragment;

    boost::spirit::qi::rule<iterator, boost::fusion::tuple<
        boost::optional<string_type> &,
        boost::optional<string_type> &,
        boost::optional<boost::uint16_t> &,
        string_type &
            >()> hier_part;
};

} // namespace uri

#endif

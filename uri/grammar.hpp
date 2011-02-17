#ifndef URI_GRAMMAR_HPP
#define URI_GRAMMAR_HPP

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright 2011 Steven Siloti.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <uri/basic_uri.hpp>

#include <uri/path_grammars.hpp>
#include <uri/authority_grammar.hpp>

#include <boost/spirit/home/qi/nonterminal/rule.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

#include <boost/spirit/home/qi/directive/raw.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>

#include <boost/spirit/home/qi/operator.hpp>

#include <boost/spirit/home/qi/char/char_class.hpp>
#include <boost/spirit/home/qi/char/char.hpp>
#include <boost/spirit/home/qi/string/lit.hpp>

#include <boost/spirit/home/qi/auxiliary/attr.hpp>

namespace uri {

namespace qi = boost::spirit::qi;

template <typename Iterator, class String>
struct grammar
    : path_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, basic_uri<String>()>
{
    typedef String string_type;

    grammar() : grammar::base_type(start, "uri")
    {
        // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        scheme %= qi::alpha >> *(qi::alnum | qi::char_("+.-"));

        // query = *( pchar / "/" / "?" )
        query %= qi::raw[*(pchar | qi::char_("/?"))];
        // fragment = *( pchar / "/" / "?" )
        fragment %= qi::raw[*(pchar | qi::char_("/?"))];

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

    boost::spirit::qi::rule<Iterator, string_type()>
        scheme, query, fragment;

    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<string_type> >&,
        string_type &
    >()> hier_part;

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, basic_uri<string_type>()> start;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename detail::basic_uri_tuple<string_type>::type()> uri;

    authority_grammar<Iterator, string_type> authority;

    path_abempty_grammar<Iterator, string_type> path_abempty;
    path_absolute_grammar<Iterator, string_type> path_absolute;
    path_rootless_grammar<Iterator, string_type> path_rootless;
};

} // namespace uri

#endif // ifdef URI_GRAMMAR_HPP

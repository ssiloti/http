//
// components.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_COMPONENTS_HPP
#define URI_COMPONENTS_HPP

#include <uri/authority_grammar.hpp>
#include <uri/path_rules.hpp>

#include <boost/spirit/home/qi/string/lit.hpp>
#include <boost/spirit/home/qi/auxiliary/attr.hpp>
#include <boost/spirit/home/qi/auxiliary/eps.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

#include <boost/fusion/tuple.hpp>

#include <boost/optional.hpp>

namespace uri {

template <typename Iterator, typename String>
struct path_abempty_grammar
    : public path_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
public:
    typedef String string_type;

    path_abempty_grammar() : path_abempty_grammar::base_type(start, "path_abempty")
    {
        using namespace boost::spirit;

        // path-abempty  = *( "/" segment )
        start %= qi::raw[*(qi::char_("/") >> segment)];
    }

    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct path_absolute_grammar
    : public path_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
public:
    typedef String string_type;

    path_absolute_grammar() : path_absolute_grammar::base_type(start, "path_absolute")
    {
        using namespace boost::spirit;

        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        start %= qi::raw[
                qi::char_("/")
            >>  -(segment_nz >> *(qi::char_("/") >> segment))
        ];
    }

    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct path_rootless_grammar
    : public path_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
public:
    typedef String string_type;

    path_rootless_grammar() : path_rootless_grammar::base_type(start, "path_rootless")
    {
        using namespace boost::spirit;

        // path-rootless = segment-nz *( "/" segment )
        start %= qi::raw[
            segment_nz >> *(qi::char_("/") >> segment)
        ];
    }

    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct query_grammar
    : character_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
public:
    typedef String string_type;

    query_grammar() : query_grammar::base_type(start, "query")
    {
        using namespace boost::spirit;

        // query = *( pchar / "/" / "?" )
        start %= qi::raw[*(pchar | qi::char_("/?"))];
    }

    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct fragment_grammar
    : character_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
public:
    typedef String string_type;

    fragment_grammar() : fragment_grammar::base_type(start, "fragment")
    {
        using namespace boost::spirit;

        // fragment = *( pchar / "/" / "?" )
        start %= qi::raw[*(pchar | qi::char_("/?"))];
    }

    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct scheme_grammar
    : character_rules<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
public:
    typedef String string_type;

    scheme_grammar() : scheme_grammar::base_type(start, "scheme")
    {
        using namespace boost::spirit;

        // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        start %= qi::alpha >> *(qi::alnum | qi::char_("+.-"));
    }

    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct hier_part_grammar
    : boost::spirit::qi::grammar<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<String> >&,
        String &
    >()>
{
public:
    typedef String string_type;

    hier_part_grammar() : hier_part_grammar::base_type(start, "hier_part")
    {
        using namespace boost::spirit;

        // path-empty = 0<pchar>
        path_empty %= qi::eps;

        // hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
        // Use eps(false) to force the authority attribute to be optional
        // Boost <= 1.45 was a-ok without it *shrug*
        start %=
                (qi::lit("//") >> (qi::eps(false) | authority) >> path_abempty)
            |
                (
                    qi::attr(boost::optional<basic_authority<string_type> >())
                >>  (path_absolute | path_rootless | path_empty)
                );
    }

    authority_grammar<Iterator, string_type> authority;

    path_abempty_grammar<Iterator, string_type> path_abempty;
    path_absolute_grammar<Iterator, string_type> path_absolute;
    path_rootless_grammar<Iterator, string_type> path_rootless;

    boost::spirit::qi::rule<Iterator, string_type()> path_empty;

    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<string_type> >&,
        string_type &
    >()> start;
};

}

#endif
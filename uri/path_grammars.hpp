//
// path_grammars.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_PATH_GRAMMARS
#define URI_PATH_GRAMMARS

#include <uri/path_rules.hpp>

#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

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

}

#endif
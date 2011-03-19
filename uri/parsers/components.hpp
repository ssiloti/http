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

#ifndef URI_PARSERS_COMPONENTS_HPP
#define URI_PARSERS_COMPONENTS_HPP

#include <uri/parsers/authority.hpp>
#include <uri/parsers/segments.hpp>

#include <boost/spirit/home/qi/string/lit.hpp>
#include <boost/spirit/home/qi/auxiliary/attr.hpp>
#include <boost/spirit/home/qi/auxiliary/eps.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>

#include <boost/fusion/tuple.hpp>

#include <boost/optional.hpp>

namespace uri { namespace parsers {

template <typename Iterator, typename String>
struct path_abempty
    : public segments<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    path_abempty() : path_abempty::base_type(start, "path_abempty")
    {
        using namespace boost::spirit;

        // path-abempty  = *( "/" segment )
        start %= qi::raw[*(qi::char_("/") >> this->segment)];
    }

private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct path_absolute
    : public segments<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    path_absolute() : path_absolute::base_type(start, "path_absolute")
    {
        using namespace boost::spirit;

        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        start %= qi::raw[
                qi::char_("/")
            >>  -(this->segment_nz >> *(qi::char_("/") >> this->segment))
        ];
    }

private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct path_rootless
    : public segments<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    path_rootless() : path_rootless::base_type(start, "path_rootless")
    {
        using namespace boost::spirit;

        // path-rootless = segment-nz *( "/" segment )
        start %= qi::raw[
            this->segment_nz >> *(qi::char_("/") >> this->segment)
        ];
    }
    
private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct query
    : characters<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    query() : query::base_type(start, "query")
    {
        using namespace boost::spirit;

        // query = *( pchar / "/" / "?" )
        start %= qi::raw[*(this->pchar | qi::char_("/?"))];
    }
    
private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct fragment
    : characters<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    fragment() : fragment::base_type(start, "fragment")
    {
        using namespace boost::spirit;

        // fragment = *( pchar / "/" / "?" )
        start %= qi::raw[*(this->pchar | qi::char_("/?"))];
    }
    
private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct scheme
    : characters<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    scheme() : scheme::base_type(start, "scheme")
    {
        using namespace boost::spirit;

        // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        start %= qi::alpha >> *(qi::alnum | qi::char_("+.-"));
    }
    
private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct hier_part
    : boost::spirit::qi::grammar<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<String> >&,
        String &
    >()>
{
    typedef String string_type;

    hier_part() : hier_part::base_type(start_, "hier_part")
    {
        using namespace boost::spirit;

        // path-empty = 0<pchar>
        path_empty_ %= qi::eps;

        // hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
        // Use eps(false) to force the authority attribute to be optional
        // Boost <= 1.45 was a-ok without it *shrug*
        start_ %=
                (qi::lit("//") >> (qi::eps(false) | authority_) >> path_abempty_)
            |
                (
                    qi::attr(boost::optional<basic_authority<string_type> >())
                >>  (path_absolute_ | path_rootless_ | path_empty_)
                );
    }

private:
    authority<Iterator, string_type> authority_;

    path_abempty<Iterator, string_type> path_abempty_;
    path_absolute<Iterator, string_type> path_absolute_;
    path_rootless<Iterator, string_type> path_rootless_;

    boost::spirit::qi::rule<Iterator, string_type()> path_empty_;

    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<string_type> >&,
        string_type &
    >()> start_;
};

} }

#endif

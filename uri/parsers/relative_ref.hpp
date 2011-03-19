//
// relative_ref.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_PARSERS_RELATIVE_REF_HPP
#define URI_PARSERS_RELATIVE_REF_HPP

#include <uri/parsers/components.hpp>

namespace uri { namespace parsers {

template <typename Iterator, typename String>
struct path_noscheme
    : public segments<Iterator, String>
    , boost::spirit::qi::grammar<Iterator, String()>
{
    typedef String string_type;

    path_noscheme() : path_noscheme::base_type(start, "path_noscheme")
    {
        using namespace boost::spirit;

        // path-rootless = segment-nz *( "/" segment )
        start %= qi::raw[
            segment_nz_nc >> *(qi::char_("/") >> segment)
        ];
    }
    
private:
    boost::spirit::qi::rule<Iterator, string_type()> start;
};

template <typename Iterator, typename String>
struct relative_part
    : boost::spirit::qi::grammar<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<String> >&,
        String &
    >()>
{
    typedef String string_type;

    relative_part() : relative_part::base_type(start, "relative_part")
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
                >>  (path_absolute | path_noscheme | path_empty)
                );
    }

private:
    authority<Iterator, string_type> authority;

    path_abempty<Iterator, string_type> path_abempty;
    path_absolute<Iterator, string_type> path_absolute;
    path_noscheme<Iterator, string_type> path_noscheme;

    boost::spirit::qi::rule<Iterator, string_type()> path_empty;

    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<
        boost::optional<basic_authority<string_type> >&,
        string_type &
    >()> start;
};

template <typename Iterator, typename String>
struct relative_ref {
// TODO
};

} }

#endif

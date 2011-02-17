//
// path_rules.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_PATH_RULES
#define URI_PATH_RULES

#include <uri/character_rules.hpp>

#include <boost/spirit/home/qi/directive/raw.hpp>
#include <boost/spirit/home/qi/auxiliary/eps.hpp>

namespace uri {

template <typename Iterator, typename String>
struct path_rules : public character_rules<Iterator, String>
{
public:
    typedef String string_type;

    path_rules()
    {
        using namespace boost::spirit;

        // segment = *pchar
        segment %= qi::raw[*pchar];

        // segment-nz = 1*pchar
        segment_nz %= qi::raw[+pchar];

        // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
        segment_nz_nc %= qi::raw[
            +(unreserved | pct_encoded | sub_delims | qi::char_("@"))
        ];

        // path-empty = 0<pchar>
        path_empty %= qi::eps;
    }

    boost::spirit::qi::rule<Iterator, string_type()>
        segment, segment_nz, segment_nz_nc;

    boost::spirit::qi::rule<Iterator, string_type()>
        path_empty;
};

}

#endif

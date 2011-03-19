//
// segments.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_PARSERS_SEGMENTS_HPP
#define URI_PARSERS_SEGMENTS_HPP

#include <uri/parsers/characters.hpp>

#include <boost/spirit/home/qi/directive/raw.hpp>
#include <boost/spirit/home/qi/auxiliary/eps.hpp>

namespace uri { namespace parsers {

template <typename Iterator, typename String>
struct segments : public characters<Iterator, String>
{
    typedef String string_type;

    segments()
    {
        using namespace boost::spirit;

        // segment = *pchar
        segment %= qi::raw[*this->pchar];

        // segment-nz = 1*pchar
        segment_nz %= qi::raw[+this->pchar];

        // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
        segment_nz_nc %= qi::raw[
            +(this->unreserved | this->pct_encoded | this->sub_delims | qi::char_("@"))
        ];
    }

    boost::spirit::qi::rule<Iterator, string_type()>
        segment, segment_nz, segment_nz_nc;
};

} }

#endif

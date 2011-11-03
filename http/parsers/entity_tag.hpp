//
// entity_tag.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_PARSERS_ENTITY_TAG_HPP
#define HTTP_PARSERS_ENTITY_TAG_HPP

#include <http/entity_tag.hpp>
#include <http/parsers/basic_rules.hpp>
#include <boost/spirit/home/qi/auxiliary/attr.hpp>

namespace http { namespace parsers {

template <typename Iterator>
struct entity_tag : boost::spirit::qi::grammar<Iterator, http::entity_tag()>
{
    entity_tag() : entity_tag::base_type(start, "entity_tag")
    {
        using namespace boost::spirit::qi;

        start = ((lit("W/") >> attr(true)) | attr(false)) >> b.quoted_string;
    }

    basic_rules<Iterator> b;
    boost::spirit::qi::rule<Iterator, http::entity_tag()> start;
};

} }

#endif

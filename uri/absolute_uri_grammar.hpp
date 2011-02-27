//
// absolute_uri_grammar.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef URI_ABSOLUTE_URI_HPP
#define URI_ABSOLUTE_URI_HPP

#include <uri/basic_uri.hpp>

#include <uri/components.hpp>

namespace uri {

namespace qi = boost::spirit::qi;

template <typename Iterator, class String>
struct absolute_uri_grammar
    : boost::spirit::qi::grammar<Iterator, basic_uri<String>()>
{
    typedef String string_type;

    absolute_uri_grammar() : absolute_uri_grammar::base_type(start, "absolute_uri")
    {
        uri %=
                scheme >> ':'
            >>  hier_part
            >>  -('?' >> query)
            >>  qi::attr(boost::optional<string_type>());

        start %= uri.alias();
    }

    scheme_grammar<Iterator, string_type> scheme;
    hier_part_grammar<Iterator, string_type> hier_part;
    query_grammar<Iterator, string_type> query;

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, basic_uri<string_type>()> start;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename detail::basic_uri_tuple<string_type>::type()> uri;
};

}

#endif

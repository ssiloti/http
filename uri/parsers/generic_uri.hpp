//
// generic_uri.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef URI_PARSERS_GRAMMAR_HPP
#define URI_PARSERS_GRAMMAR_HPP

#include <uri/basic_uri.hpp>
#include <uri/parsers/components.hpp>

namespace uri { namespace parsers {

template <typename Iterator, class String>
struct generic_uri
    : boost::spirit::qi::grammar<Iterator, basic_uri<String>()>
{
    typedef String string_type;

    generic_uri() : generic_uri::base_type(start, "generic_uri")
    {
        uri %=
                scheme >> ':'
            >>  hier_part
            >>  -('?' >> query)
            >>  -('#' >> fragment);

        start %= uri.alias();
    }

private:
    scheme<Iterator, string_type> scheme;
    hier_part<Iterator, string_type> hier_part;
    query<Iterator, string_type> query;
    fragment<Iterator, string_type> fragment;

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, basic_uri<string_type>()> start;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename detail::basic_uri_tuple<string_type>::type()> uri;


};

} } // namespace parsers namespace uri

#endif // ifdef URI_GRAMMAR_HPP

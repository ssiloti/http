//
// absolute_uri.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef URI_PARSERS_ABSOLUTE_URI_HPP
#define URI_PARSERS_ABSOLUTE_URI_HPP

#include <uri/basic_uri.hpp>
#include <uri/parsers/components.hpp>

namespace uri { namespace parsers {

template <typename Iterator, class String>
struct absolute_uri
    : boost::spirit::qi::grammar<Iterator, basic_uri<String>()>
{
    typedef String string_type;

    absolute_uri() : absolute_uri::base_type(start_, "absolute_uri")
    {
        uri_ %=
                scheme_ >> ':'
            >>  hier_part_
            >>  -('?' >> query_)
            >>  boost::spirit::qi::attr(boost::optional<string_type>());

        start_ %= uri_.alias();
    }

private:
    scheme<Iterator, string_type> scheme_;
    hier_part<Iterator, string_type> hier_part_;
    query<Iterator, string_type> query_;

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, basic_uri<string_type>()> start_;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename detail::basic_uri_tuple<string_type>::type()> uri_;
};

} }

#endif

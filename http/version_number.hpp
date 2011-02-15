//
// version_number.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_VERSION_NUMBER_HPP
#define HTTP_VERSION_NUMBER_HPP

#include <boost/spirit/home/qi/numeric.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace http {

struct version_number
{
    template <typename Iterator>
    static boost::spirit::qi::rule<Iterator, version_number()> parse_rule();
    
    int major, minor;
};

}

BOOST_FUSION_ADAPT_STRUCT(
    http::version_number,
    (int, major)
    (int, minor)
)

namespace http {

template <typename Iterator>
boost::spirit::qi::rule<Iterator, version_number()> version_number::parse_rule()
{
    using namespace boost::spirit::qi;
    static rule<Iterator, version_number()> rule;
    rule = lit("HTTP/") >> int_ >> '.' >> int_;
    return rule;
}

}

#endif

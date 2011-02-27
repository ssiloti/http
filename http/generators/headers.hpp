//
// headers.hpp
// ~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_GENERATORS_HEADERS_HPP
#define HTTP_GENERATORS_HEADERS_HPP

#include <http/headers.hpp>

#include <boost/spirit/include/karma_generate_auto.hpp>
#include <boost/spirit/home/karma/numeric.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>

namespace http {
namespace generators {

template <typename Header, typename OutputIterator>
bool generate_header(const Header& header, OutputIterator& sink)
{
    return boost::spirit::karma::generate(sink, header.second);
}

template <typename OutputIterator>
bool generate_header(const headers::allow& header, OutputIterator& sink)
{
    return boost::spirit::karma::generate(sink, boost::spirit::karma::string % ',', header.second);
}

/*
template <typename OutputIterator>
bool generate_header(headers::& header, OutputIterator& sink)
{
    return boost::spirit::karma::generate(sink, header.second);
}

template <typename OutputIterator>
bool generate_header(headers::& header, OutputIterator& sink)
{
    return boost::spirit::karma::generate(sink, header.second);
}
*/

} } // namespace generators namespace http

#endif

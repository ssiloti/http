//
// response.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_GENERATORS_RESPONSE_HPP
#define HTTP_GENERATORS_RESPONSE_HPP

#include <http/basic_response.hpp>
#include <http/generators/version_number.hpp>

#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>
#include <boost/spirit/include/karma_generate.hpp>

#include <boost/fusion/tuple/make_tuple.hpp>
#include <boost/ref.hpp>

namespace http {

template <typename Headers, typename Body>
template <typename OutputIterator>
bool basic_response<Headers, Body>::generate_start_line(OutputIterator sink) const
{
    using namespace boost::spirit::karma;
    return generate(sink,
        generators::version_number_grammar<OutputIterator>() << ' '
        << uint_ << ' ' << string << "\r\n",
        boost::fusion::make_tuple(this->version, unsigned(status), boost::cref(reason)));
}

} // namespace http

#endif

//
// request.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_GENERATORS_REQUEST_HPP
#define HTTP_GENERATORS_REQUEST_HPP

#include <http/generators/version_number.hpp>
#include <http/basic_request.hpp>

#include <uri/generators/generic_uri.hpp>
#include <uri/generators/authority.hpp>

#include <boost/spirit/home/karma/directive/buffer.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>
#include <boost/spirit/include/karma_generate.hpp>

#include <boost/fusion/tuple/make_tuple.hpp>
#include <boost/ref.hpp>

namespace http {

template <typename Headers, typename Body>
template <typename OutputIterator>
bool basic_request<Headers, Body>::generate_start_line(OutputIterator sink) const
{
    using namespace boost::spirit::karma;
    rule<OutputIterator, char()> astk_lit = lit('*');
    rule<OutputIterator, asterisk()> astk = astk_lit.alias();

    return generate(sink,
        string << ' '
            << (  astk
                | uri::generators::generic_uri<OutputIterator, std::string>()
                | (string << buffer[-('?' << string)])
                | uri::generators::authority<OutputIterator, std::string>())
            << ' ' << generators::version_number_grammar<OutputIterator>(),
       boost::fusion::make_tuple(boost::cref(method), target, version));
}

}

#endif

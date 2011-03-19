//
// message.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_MESSAGE_IO_HPP
#define HTTP_MESSAGE_IO_HPP

#include <http/basic_message.hpp>
#include <http/parsers/basic_rules.hpp>

namespace http {

template <typename Headers, typename Body>
template <typename InputIterator>
bool basic_message<Headers, Body>::parse_header(InputIterator begin, InputIterator sep, InputIterator end)
{
    parsers::basic_rules<InputIterator> b;
    std::string name;
    if (boost::spirit::qi::phrase_parse(begin, sep, b.token, b.skipper, name)) {
        headers.append(name, ++sep, end);
        return true;
    }
    else
        return false;
}

} // namespace http

#endif

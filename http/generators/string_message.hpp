//
// string_message.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_GENERATORS_STRING_MESSAGE_HPP
#define HTTP_GENERATORS_STRING_MESSAGE_HPP

#include <http/headers.hpp>

#include <boost/spirit/include/karma_generate_auto.hpp>
#include <boost/spirit/home/karma/numeric.hpp>

#include <boost/mpl/string.hpp>
#include <boost/mpl/size.hpp>

#include <string>
#include <algorithm>

namespace http { namespace generators {

boost::fusion::map<headers::content_length> get_headers(const std::string& str)
{
    return boost::fusion::map<headers::content_length>(headers::content_length(str.length()));
}

template <typename OutputIterator>
bool generate_body(OutputIterator sink, const std::string& str)
{
    std::copy(str.begin(), str.end(), sink);
    return true;
}

} }

#endif

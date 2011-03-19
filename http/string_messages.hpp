//
// string_messages.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_STRING_MESSAGES_HPP
#define HTTP_STRING_MESSAGES_HPP

#include <http/basic_request.hpp>
#include <http/basic_response.hpp>
#include <http/headers.hpp>

#include <string>
#include <algorithm>

namespace http {

typedef basic_request<headers::map, std::string> string_request;
typedef basic_response<headers::map, std::string> string_response;

namespace generators {

template <typename OutputIterator>
bool generate_body(OutputIterator sink, const std::string& str)
{
    std::copy(str.begin(), str.end(), sink);
    return true;
}

}

}

#endif

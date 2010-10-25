//
// basic_message.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_MESSAGE_HPP
#define HTTP_BASIC_MESSAGE_HPP

#include <http/version_number.hpp>
#include <http/headers_map.hpp>

namespace http {

template <typename Headers, typename Body>
class basic_message
{
public:
	typedef headers_map<Headers> headers_type;
	typedef Body entity_body_type;

	version_number version;
	headers_type headers;
	entity_body_type body;
};

}

#endif

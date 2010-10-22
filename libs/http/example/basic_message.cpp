//
// basic_message.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <http/headers/content_length.hpp>
#include <http/headers/allow.hpp>
#include <http/basic_message.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/mpl/string.hpp>

#include <iostream>

int main()
{
	namespace f = boost::fusion;
	namespace m = boost::mpl;
	using namespace http::headers;

	typedef f::map<content_length, allow> headers;

	http::basic_message<headers, void> msg;

	msg.header<content_length>() = 1234;
	msg.header("allow", "GET, POST, PUT");
	std::cout << msg.header("content-length")[0] << std::endl;
	std::cout << msg.header("allow")[0];
}

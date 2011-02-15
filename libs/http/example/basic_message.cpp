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
#include <http/basic_request.hpp>

#include <boost/mpl/map.hpp>

#include <iostream>

int main()
{
	namespace m = boost::mpl;
	using namespace http::headers;

	typedef m::map<content_length, allow> headers;

	http::basic_message<headers, std::string> msg;

	msg.headers.at<content_length>() = 1234;
	msg.headers.insert(http::headers_map<headers>::value_type("allow", "GET, POST, PUT"));
	std::cout << msg.headers.find("content-length")->second.operator std::string() << std::endl;
	std::cout << msg.headers.find("allow")->second.operator std::string();

    uri::http_uri<std::string> test_uri;
    uri::parse(std::string("http://example.com"), test_uri);
}

//
// basic_message.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <http/headers.hpp>
#include <http/basic_message.hpp>
#include <http/basic_request.hpp>

#include <http/parsers/headers.hpp>
#include <http/generators/headers.hpp>

#include <boost/fusion/container/map.hpp>

#include <iostream>

int main()
{
    using namespace http::headers;

    typedef boost::fusion::map<content_length, allow> headers;

    http::basic_message<headers, std::string> msg;

    msg.headers.at<content_length>() = 1234;
    msg.headers.insert("allow", "GET, POST, PUT");
    std::cout << msg.headers.find("content-length")->second.to_string() << std::endl;
    std::cout << msg.headers.find("allow")->second.to_string();
}

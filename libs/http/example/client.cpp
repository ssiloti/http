//
// client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <http/parsers/headers.hpp>
#include <http/generators/headers.hpp>

#include <http/parsers/message.hpp>
#include <http/parsers/response.hpp>
#include <http/generators/message.hpp>
#include <http/generators/request.hpp>

#include <http/generators/string_message.hpp>
#include <http/parsers/string_message.hpp>

#include <http/headers.hpp>

#include <http/uri_ops.hpp>
#include <uri/basic_uri.ipp>

#include <iostream>

void handle_response(const boost::system::error_code& error, boost::shared_ptr<http::string_response> res)
{
    std::cout << res->body;
}

int main()
{
    boost::asio::io_service service;
    http::async_request<std::string>(service, "GET", "http://127.0.0.1:1234", handle_response);
    service.run();
}

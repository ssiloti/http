//
// server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <http/async_string_server.hpp>
#include <http/string_message.hpp>
#include <http/generators/string_message.hpp>

#include <boost/make_shared.hpp>

class example_server : public http::async_string_server
{
public:
    example_server(boost::asio::io_service& io, boost::asio::ip::tcp::endpoint listen)
        : http::async_string_server(io, listen)
    {}

protected:
    virtual void incoming_request(http::async_string_server::context_type ctx)
    {
        boost::shared_ptr<http::string_response> response(boost::make_shared<http::string_response>());
        response->body = "Hello, World";
        response->headers.at<http::headers::content_type>() = "text/plain";
        ctx.write_response(response);
    }

};


int main()
{
    boost::asio::io_service service;
    boost::shared_ptr<example_server> example(boost::make_shared<example_server>(boost::ref(service), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1234)));
    example->start();
    service.run();
}

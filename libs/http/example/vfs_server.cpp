//
// server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <http/basic_server_impl.hpp>
#include <http/basic_async_server.hpp>

#include <http/string_message.hpp>
#include <http/generators/string_message.hpp>

#include <http/vfs/server.hpp>

#include <boost/make_shared.hpp>

typedef http::vfs::server<http::basic_async_server<http::headers::map, std::string> > server_type;

class example_server : public server_type
{
public:
    example_server(boost::asio::io_service& io, boost::asio::ip::tcp::endpoint listen)
        : server_type(io, listen)
    {}
};

class example_node : public http::vfs::node<server_type::context_type>
{
    public:
    virtual void dispatch(context_type ctx,
                          const std::string& path,
                          std::string::const_iterator segment_begin,
                          std::string::const_iterator segment_end)
    {
        boost::shared_ptr<http::string_response> response(boost::make_shared<http::string_response>());
        response->body = "Hello, World";
        response->headers.at<http::headers::content_type>() = "text/plain";
        ctx.connection->write_response(response);
    }
};

int main()
{
    boost::asio::io_service service;
    boost::shared_ptr<example_server> example(boost::make_shared<example_server>(boost::ref(service), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1234)));
    example_node hello_node;
    example->add_node("hello", hello_node);
    example->start();
    service.run();
}

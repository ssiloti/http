//
// uri_ops.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_URI_OPS_HPP
#define HTTP_URI_OPS_HPP

#include <http/string_message.hpp>
#include <http/async_client_connection.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
//#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

namespace http {

namespace detail {

template <typename String, typename Handler>
class request_op : public boost::enable_shared_from_this<request_op<String, Handler> >
{
    typedef request_op<String, Handler> this_type;
    typedef String string_type;
    typedef async_client_connection<boost::asio::ip::tcp::socket> connection_type;

public:
    request_op(boost::asio::io_service& io_service, Handler h) : handler_(h), resolver_(io_service)
    {
        connection_ = boost::make_shared<connection_type>(io_service);
    }

    static void create(boost::asio::io_service& io_service,
                       const string_type& method,
                       const uri::basic_uri<String>& res,
                       Handler handler)
    {
        boost::shared_ptr<this_type> op(boost::make_shared<this_type>(io_service, handler));

        op->request_.method = method;
        op->request_.target = res;
        op->request_.headers.template at<headers::content_length>() = 0;

        boost::asio::ip::tcp::resolver::query::flags qflags = boost::asio::ip::tcp::resolver::query::address_configured;
        std::string service(res.scheme);
        if (res.authority->port) {
            service = boost::lexical_cast<std::string>(*res.authority->port);
            qflags |= boost::asio::ip::tcp::resolver::query::numeric_service;
        }
        boost::asio::ip::tcp::resolver::query q(res.authority->host, service, qflags);
        op->resolver_.async_resolve(q, boost::bind(&this_type::handle_resolved, op, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    }

private:
    void handle_resolved(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator iterator)
    {
        if (!error)
        {
            // Attempt a connection to the first endpoint in the list. Each endpoint
            // will be tried until we successfully establish a connection.
            boost::asio::ip::tcp::endpoint endpoint = *iterator;
            connection_->next_layer().async_connect(endpoint,
                boost::bind(&this_type::handle_connect, this->shared_from_this(),
                boost::asio::placeholders::error, ++iterator));
        }
    }

    void handle_connect(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator iterator)
    {
        using namespace boost::asio;

        if (!error)
        {
            response_ = boost::make_shared<string_response>();
            connection_->write_request(request_,
                                      *response_,
                                      boost::protect(boost::bind(&this_type::handle_response, this->shared_from_this(), placeholders::error)));
        }
        else if (iterator != ip::tcp::resolver::iterator())
        {
            // The connection failed. Try the next endpoint in the list.
            connection_->next_layer().close();
            ip::tcp::endpoint endpoint = *iterator;
            connection_->next_layer().async_connect(
                endpoint,
                boost::bind(&this_type::handle_connect, this, placeholders::error, ++iterator));
        }
    }

    void handle_response(const boost::system::error_code& error)
    {
        handler_(error, response_);
    }

    Handler handler_;
    boost::shared_ptr<connection_type> connection_;
    boost::asio::ip::tcp::resolver resolver_;
    basic_request<headers::map, String> request_;
    // The response must be separately allocated so that it can out-live the operation
    boost::shared_ptr<string_response> response_;
};

}

template <typename String, typename Handler>
void async_request(boost::asio::io_service& io_service,
                   const String& method,
                   const uri::basic_uri<String>& res,
                   Handler handler)
{
    detail::request_op<String, Handler>::create(io_service, method, res, handler);
}

}

#endif

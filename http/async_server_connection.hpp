//
// async_server_connection.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ASYNC_SERVER_CONNECTION_HPP
#define HTTP_ASYNC_SERVER_CONNECTION_HPP

#include <http/async_connection.hpp>
#include <http/basic_request.hpp>
#include <http/parsers/message_state.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/variant/variant.hpp>

#include <boost/smart_ptr/enable_shared_from_this.hpp>

#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <deque>
#include <vector>
#include <algorithm>
#include <iterator>

namespace http {

namespace ip = boost::asio::ip;
namespace placeholders = boost::asio::placeholders;

template <typename Stream, typename Headers, typename Body>
class async_server_connection : public async_connection<Stream>
{
    typedef async_server_connection<Stream, Headers, Body> this_type;

public:
    typedef basic_request<Headers, Body> request_type;

    boost::shared_ptr<this_type> shared_from_this()
    {
        return boost::static_pointer_cast<this_type>(async_connection<Stream>::shared_from_this());
    }

    boost::shared_ptr<const this_type> shared_from_this() const
    {
        return boost::static_pointer_cast<const this_type>(async_connection<Stream>::shared_from_this());
    }

public:
    struct context_type
    {
        context_type(
            boost::shared_ptr<this_type> con,
            const request_type& req)
            : connection(con), request(req)
        {}

        boost::shared_ptr<this_type> connection;
        const request_type& request;
    };

public:
    typedef typename recv_buffer_t::const_iterator parse_iterator_type;

    template <typename Handler>
    async_server_connection(ip::tcp::acceptor& acceptor, Handler handler)
        : async_connection<Stream>(acceptor.get_io_service())
        , active_request_state_(active_request_)
    {
        acceptor.async_accept(socket_, handler);
    }

    const std::string& remote_host_name() { return remote_host_name_; }

    template <typename Handler>
    void read_request(Handler handler)
    {
        active_request_.clear();
        read_message(
            boost::system::error_code(),
            0,
            &active_request_state_,
            boost::protect(boost::bind(
                handler,
                placeholders::error,
                context_type(shared_from_this(), active_request_)
            ))
        );
    }

    template <typename Response>
    void write_response(Response response)
    {
        active_request_state_.reset();
        send_queue_.push_back(
            boost::bind(
                &this_type::write_next_response<Response>,
                this,
                response
            )
        );
        
        if (send_queue_.size() == 1)
            send_queue_.back()();
    }

protected:
    template <typename Response>
    void write_next_response(Response response)
    {
        generation_iterator sink(start_generation());
        generators::generate_message(sink, response);
        boost::asio::async_write(
            socket_,
            vectorize(sink),
            boost::bind(
                &this_type::response_written,
                this->shared_from_this(),
                placeholders::error,
                placeholders::bytes_transferred
            )
        );
    }

    void response_written(const boost::system::error_code& error, std::size_t bytes_transfered)
    {
        if (error) {
            socket_.close();
            return;
        }

        send_queue_.pop_front();
        if (!send_queue_.empty())
            send_queue_.back()();
    }


    request_type active_request_;
    parsers::message_state<request_type, typename recv_buffer_t::iterator> active_request_state_;
    std::string remote_host_name_;
};

}

#endif

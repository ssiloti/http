//
// async_client_connection.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ASYNC_CLIENT_CONNECTION_HPP
#define HTTP_ASYNC_CLIENT_CONNECTION_HPP

#include <http/async_connection.hpp>
#include <http/parsers/message_state.hpp>

#include <boost/asio/write.hpp>
#include <boost/bind/protect.hpp>
#include <boost/make_shared.hpp>

namespace http {

template <typename Stream>
class async_client_connection : public async_connection<Stream>
{
    typedef async_client_connection<Stream> this_type;
    typedef async_connection<Stream> base_type;
    typedef std::deque<boost::function<void()> > send_queue_t;
    typedef std::deque<boost::function<void()> > receive_queue_t;

    typedef std::vector<boost::uint8_t> send_buffer_t;

public:

    async_client_connection(boost::asio::io_service& io_service)
        : async_connection<Stream>(io_service)
    {
    }

    template <typename Request, typename Response, typename Handler>
    void write_request(const Request& request, Response& response, Handler handler)
    {
        send_queue_.push_back(
            boost::bind(
                &this_type::write_next_request<Request, Response, Handler>,
                this,    // does not need to be a shared_ptr since the container we're storing it in is a member of this
                boost::cref(request),
                boost::ref(response),
                handler
            )
        );
        
        if (send_queue_.size() == 1)
            send_queue_.front()();
    }

    boost::shared_ptr<this_type> shared_from_this()
    {
        return boost::static_pointer_cast<this_type>(async_connection<Stream>::shared_from_this());
    }

    boost::shared_ptr<const this_type> shared_from_this() const
    {
        return boost::static_pointer_cast<const this_type>(async_connection<Stream>::shared_from_this());
    }

private:
    template <typename Request, typename Response, typename Handler>
    void write_next_request(const Request& request, Response& response, Handler handler)
    {
        using namespace boost::asio;

        typename base_type::generation_iterator sink(this->start_generation());
        generators::generate_message(sink, request);
        async_write(
            this->socket_,
            this->vectorize(sink),
            boost::bind(
                &this_type::request_written<Response, Handler>,
                this->shared_from_this(),
                placeholders::error,
                placeholders::bytes_transferred,
                boost::ref(response),
                handler
            )
        );
    }

    template <typename Response, typename Handler>
    void request_written(const boost::system::error_code& error,
                         std::size_t bytes_transfered,
                         Response& response,
                         Handler handler)
    {
        if (error) {
            this->socket_.close();
            handler(error);
            return;
        }

        send_queue_.pop_front();
        if (!send_queue_.empty())
            send_queue_.front()();

        receive_queue_.push_back(
            boost::bind(
                &this_type::read_next_response<Response, Handler>,
                this,    // does not need to be a shared_ptr since the container we're storing it in is a member of this
                boost::ref(response),
                handler
            )
        );

        if (receive_queue_.size() == 1)
            receive_queue_.front()();
    }
    
    template <typename Response, typename Handler>
    void read_next_response(Response& response, Handler handler)
    {
        read_message(boost::system::error_code(),
                     0,
                     boost::make_shared<parsers::message_state<Response, typename base_type::recv_buffer_t::iterator> >(response),
                     boost::protect(boost::bind(&this_type::response_read<Handler>, shared_from_this(), boost::asio::placeholders::error, handler)));
    }

    template <typename Handler>
    void response_read(const boost::system::error_code& error, Handler handler)
    {
        if (!error)
        {
            receive_queue_.pop_front();
            if (!receive_queue_.empty())
                receive_queue_.front()();
        }

        handler(error);
    }

    send_queue_t send_queue_;
    receive_queue_t receive_queue_;
};

}

#endif

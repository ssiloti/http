//
// basic_async_server.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_ASYNC_SERVER_HPP
#define HTTP_BASIC_ASYNC_SERVER_HPP

#include <http/async_server_connection.hpp>

#include <boost/bind/protect.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/signals2/signal.hpp>

namespace http {

template <typename Headers, typename Body>
class basic_async_server : public boost::enable_shared_from_this<basic_async_server<Headers, Body> >
{
    typedef boost::signals2::signal<void()> shutdown_sig_t;
public:
    typedef async_server_connection<Headers, Body> connection_type;

    basic_async_server(boost::asio::io_service& io, ip::tcp::endpoint listen)
        : acceptor_(io, listen)
    {}

    void start()
    {
        connections_.push_back(
            boost::make_shared<connection_type>(
                boost::ref(acceptor_),
                boost::bind(
                    &basic_async_server<Headers, Body>::handle_accepted,
                    this->shared_from_this(),
                    placeholders::error
                )
            )
        );

        shutdown_sig_.connect(
            shutdown_sig_t::slot_type(
                &connection_type::close,
                connections_.back().get()
            ).track(connections_.back())
        );
    }

    void stop()
    {
        shutdown_sig_();
        acceptor_.close();
    }

protected:
    struct context_type
    {
        context_type(
            boost::shared_ptr<basic_async_server<Headers, Body> > serv,
            boost::shared_ptr<const connection_type> con,
            const typename connection_type::request_type& req)
            : server_(serv), connection(con), request(req)
        {}

        boost::shared_ptr<const connection_type> connection;
        const typename connection_type::request_type& request;

        template <typename Response>
        void write_response(Response response)
        {
            connection_type& con(*boost::const_pointer_cast<connection_type>(connection));
            con.write_response(response);
            con.read_request(
                boost::protect(boost::bind(
                    &basic_async_server<Headers, Body>::incoming_request,
                    server_,
                    placeholders::error,
                    _2
                ))
            );
        }

    private:
        boost::shared_ptr<basic_async_server<Headers, Body> > server_;
    };

    virtual void incoming_request(typename context_type ctx) = 0;

private:
    void handle_accepted(const boost::system::error_code& error)
    {
        if (!error) {
            connections_.back()->read_request(
                boost::protect(boost::bind(
                    &basic_async_server::incoming_request,
                    this->shared_from_this(),
                    placeholders::error,
                    _2
                ))
            );
        }

        if (acceptor_.is_open())
            start();
    }

    void incoming_request(const boost::system::error_code& error, typename connection_type::context_type ctx)
    {
        if (error)
            return;

        incoming_request(context_type(this->shared_from_this(), ctx.connection, ctx.request));
    }

    std::vector<boost::shared_ptr<connection_type> > connections_;
    ip::tcp::acceptor acceptor_;
    shutdown_sig_t shutdown_sig_;
};

}

#endif

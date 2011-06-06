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

#include <boost/noncopyable.hpp>

#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <deque>
#include <vector>
#include <algorithm>
#include <iterator>

namespace http {

namespace ip = boost::asio::ip;
namespace placeholders = boost::asio::placeholders;

template <typename Headers, typename Body>
class async_server_connection : public boost::enable_shared_from_this<async_server_connection<Headers, Body> >
{
    typedef async_server_connection<Headers, Body> this_type;

    static const int recv_buffer_size = 1024 * 16;

    typedef std::vector<boost::uint8_t> send_buffer_t;
    typedef boost::array<boost::uint8_t, recv_buffer_size> recv_buffer_t;

    typedef std::deque<boost::function<void()> > send_queue_t;

    struct generation_state
    {
        void clear()
        {
            scratch_buffer.clear();
            pushed_size = 0;
            buffers.clear();
            // Put an empty buffer at the head to store the headers
            buffers.push_back(boost::asio::const_buffer());
        }

        struct scratch_region
        {
            scratch_region() {}
            scratch_region(send_buffer_t::size_type o, send_buffer_t::size_type s)
                : offset(o), size(s)
            {}

            send_buffer_t::size_type offset, size;
        };

        typedef boost::variant<scratch_region, boost::asio::const_buffer> pushed_buffer;

        std::vector<pushed_buffer> buffers;
        send_buffer_t::size_type pushed_size;
        send_buffer_t scratch_buffer;
    };

public:
    typedef basic_request<Headers, Body> request_type;

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

    friend class generation_iterator;
    struct generation_iterator : public std::iterator<std::output_iterator_tag, boost::uint8_t>
    {
        friend class async_server_connection<Headers, Body>;
        typedef typename std::vector<typename generation_state::pushed_buffer>::size_type buffer_handle;
        typedef typename generation_state::scratch_region scratch_region;

        generation_iterator& operator*()
        {
            return *this;
        }

        generation_iterator& operator++()
        {
            return *this;
        }

        generation_iterator& operator++(int)
        {
            return *this;
        }

        generation_iterator& operator=(boost::uint8_t byte)
        {
            state_.scratch_buffer.push_back(byte);
            return *this;
        }

        generation_iterator& operator=(boost::asio::const_buffer buf)
        {
            commit_buffer();
            state_.buffers.push_back(buf);
            return *this;
        }

        generation_iterator& operator=(const generation_iterator& o)
        {
            return *this;
        }

        buffer_handle precommit_buffer()
        {
            state_.buffers.push_back(scratch_region());
            return state_.buffers.size() - 1;
        }

        send_buffer_t::size_type commit_buffer()
        {
            send_buffer_t::size_type scratch_size = state_.scratch_buffer.size() - state_.pushed_size;
            if (scratch_size)
            {
                state_.buffers.push_back(scratch_region(state_.pushed_size, scratch_size));
                state_.pushed_size = state_.scratch_buffer.size();
            }
            return scratch_size;
        }

        void commit_buffer(buffer_handle handle, boost::asio::const_buffer buf)
        {
            state_.buffers[handle] = buf;
        }

        void commit_buffer(buffer_handle handle)
        {
            state_.buffers[handle] = scratch_region(state_.pushed_size, state_.scratch_buffer.size() - state_.pushed_size);
            state_.pushed_size = state_.scratch_buffer.size();
        }

    private:
        generation_iterator(generation_state& s)
            : state_(s)
        {}

        struct iovec_visitor : public boost::static_visitor<>
        {
            iovec_visitor(generation_state& s, std::vector<boost::asio::const_buffer>& r) : state_(s), ret(r) {}

            void operator()(scratch_region r)
            {
                ret.push_back(boost::asio::const_buffer(&state_.scratch_buffer[r.offset], r.size));
            }

            void operator()(boost::asio::const_buffer buf)
            {
                ret.push_back(buf);
            }

        private:
            mutable std::vector<boost::asio::const_buffer>& ret;
            generation_state& state_;
        };

        std::vector<boost::asio::const_buffer> iovec()
        {
            std::vector<boost::asio::const_buffer> ret;
            std::for_each(state_.buffers.begin(), state_.buffers.end(), boost::apply_visitor(iovec_visitor(state_, ret)));
            return ret;
        }

        generation_state& state_;
    };

public:
    typedef typename recv_buffer_t::const_iterator parse_iterator_type;

    template <typename Handler>
    async_server_connection(ip::tcp::acceptor& acceptor, Handler handler)
        : socket_(acceptor.get_io_service())
        , valid_begin_(recv_buffer_.begin())
        , valid_end_(recv_buffer_.begin())
        , active_request_state_(active_request_)
    {
        acceptor.async_accept(socket_, handler);
    }

    bool is_open() { return socket_.is_open(); }
    const std::string& remote_host_name() { return remote_host_name_; }

    void close() { socket_.close(); }

    template <typename Handler>
    void read_request(Handler handler)
    {
        active_request_.clear();
        read_request(boost::system::error_code(), 0, handler);
    }

    template <typename Handler>
    void read_request_body(std::vector<boost::asio::mutable_buffer> buffers, Handler handler)
    {
        std::vector<boost::asio::mutable_buffer>::iterator begin = buffers.begin();
        std::size_t vaid_bytes = valid_end_ - valid_begin_;
        while (begin != buffers.end()) {
            std::size_t consumed = std::min(vaid_bytes, boost::asio::buffer_size(*begin));
            std::memcpy(boost::asio::buffer_cast<void*>(*begin),
                        static_cast<const void*>(&*valid_begin_),
                        consumed);
            valid_begin_ += consumed;
            if (consumed == boost::asio::buffer_size(*begin))
                ++begin;
            else {
                (*begin) = (*begin) + consumed;
                break;
            }
        }

        if (begin != buffers.end()) {
            buffers.erase(buffers.begin(), begin);
            boost::asio::async_read(socket_, buffers, handler);
        }
        else {
            handler(boost::system::error_code(), 0);
        }
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
    template <typename Handler>
    void read_request(const boost::system::error_code& error,
                            std::size_t bytes_transfered,
                            Handler handler)
    {
        if (error) {
//            handler(error);
            return;
        }

        valid_end_ += bytes_transfered;
        boost::tribool complete = active_request_state_.parse(valid_begin_, valid_end_);

        if (valid_begin_ == valid_end_)
            valid_begin_ = valid_end_ = recv_buffer_.begin();

        if (complete) {
            handler(error, context_type(this->shared_from_this(), active_request_));
        }
        else if (!complete) {
            handler(
                boost::system::error_code(
                    boost::system::errc::illegal_byte_sequence,
                    boost::system::generic_category()
                ),
                context_type(this->shared_from_this(), active_request_)
            );
        }
        else {
            if (valid_end_ == recv_buffer_.end()) {
                if (valid_begin_ == recv_buffer_.begin()) {
                    // The remote endpoint is trying to send us a start line or
                    // header which is larger than our receive buffer. We can't
                    // recover from this, and it probably indicates an errant or
                    // malicious host anyways so close the connection and report
                    // an error to the requester.
                    socket_.close();
                    handler(
                        boost::system::error_code(
                            boost::system::errc::illegal_byte_sequence,
                            boost::system::generic_category()
                        ),
                        context_type(this->shared_from_this(), active_request_)
                    );
                    return;
                }

                valid_end_ = std::copy(valid_begin_, valid_end_, recv_buffer_.begin());
                valid_begin_ = recv_buffer_.begin();
            }

            socket_.async_read_some(
                boost::asio::buffer(valid_end_, recv_buffer_.end() - valid_end_),
                boost::bind(
                    &this_type::read_request<Handler>,
                    this->shared_from_this(),
                    placeholders::error,
                    placeholders::bytes_transferred,
                    handler
                )
            );
        }
    }

    template <typename Response>
    void write_next_response(Response response)
    {
        send_buffer_.clear();
        generation_iterator sink(send_buffer_);
        generators::generate_message(sink, response);
        boost::asio::async_write(
            socket_,
            sink.iovec(),
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
    recv_buffer_t recv_buffer_;
    generation_state send_buffer_;
    send_queue_t send_queue_;
    typename recv_buffer_t::iterator valid_begin_, valid_end_;
    boost::asio::ip::tcp::socket socket_;
    std::string remote_host_name_;
};

}

#endif

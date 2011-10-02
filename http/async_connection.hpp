//
// async_connection.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ASYNC_CONNECTION_HPP
#define HTTP_ASYNC_CONNECTION_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/variant.hpp>
#include <boost/function.hpp>
#include <boost/array.hpp>
#include <boost/logic/tribool.hpp>

#include <vector>
#include <deque>

namespace http {

std::size_t memcpyv(std::vector<boost::asio::mutable_buffer>& dest, boost::asio::const_buffer src)
{
    std::vector<boost::asio::mutable_buffer>::iterator begin = dest.begin();
    std::size_t source_bytes = boost::asio::buffer_size(src);
    while (begin != dest.end()) {
        std::size_t consumed = std::min(boost::asio::buffer_size(src), boost::asio::buffer_size(*begin));
        std::memcpy(boost::asio::buffer_cast<void*>(*begin),
                    boost::asio::buffer_cast<const void*>(src),
                    consumed);
        src = src + consumed;
        if (consumed == boost::asio::buffer_size(*begin)) {
            ++begin;
        }
        else {
            (*begin) = (*begin) + consumed;
            break;
        }
    }

    dest.erase(dest.begin(), begin);
    return source_bytes - boost::asio::buffer_size(src);
}

template <typename Stream>
class async_connection : public boost::enable_shared_from_this<async_connection<Stream> >
{
    typedef async_connection<Stream> this_type;
    enum read_destination { dest_recv_buffer, dest_external_buffer };

public:
    typedef typename boost::remove_reference< Stream >::type next_layer_type;
    typedef typename next_layer_type::lowest_layer_type lowest_layer_type;

    next_layer_type& next_layer() { return socket_; }
    lowest_layer_type& lowest_layer() { return socket_.lowest_layer(); }

protected:
    typedef std::vector<boost::uint8_t> send_buffer_t;
    typedef std::deque<boost::function<void()> > send_queue_t;

    static const int recv_buffer_size = 1024 * 16;
    typedef boost::array<boost::uint8_t, recv_buffer_size> recv_buffer_t;

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

    async_connection(boost::asio::io_service& io_service)
        : socket_(io_service)
        , valid_begin_(recv_buffer_.begin())
        , valid_end_(recv_buffer_.begin())
    {
    }

public:
    friend class generation_iterator;
    struct generation_iterator : public std::iterator<std::output_iterator_tag, boost::uint8_t>
    {
        friend class async_connection<Stream>;
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
            iovec_visitor visitor(state_, ret);
            std::for_each(state_.buffers.begin(), state_.buffers.end(), boost::apply_visitor(visitor));
            return ret;
        }

        generation_state& state_;
    };

protected:
    generation_iterator start_generation()
    {
        send_buffer_.clear();
        return generation_iterator(send_buffer_);
    }

    std::vector<boost::asio::const_buffer> vectorize(generation_iterator iter)
    {
        return iter.iovec();
    }

    template <typename State, typename Handler>
    void read_message(const boost::system::error_code& error,
                            std::size_t bytes_transfered,
                            State message_state,
                            Handler handler)
    {
        valid_end_ += bytes_transfered;
        boost::tribool complete = message_state->parse_headers(valid_begin_, valid_end_);

        if (valid_begin_ == valid_end_)
            reset_recv_buffer();

        if (complete) {
            if (message_state->body_length_remaining())
                read_message_body(error, 0, dest_recv_buffer, message_state, handler);
            else
                handler(error);
        }
        else if (error) {
            handler(error);
        }
        else if (!complete) {
            handler(
                boost::system::error_code(
                    boost::system::errc::illegal_byte_sequence,
                    boost::system::generic_category()
                )
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
                        )
                    );
                    return;
                }

                valid_end_ = std::copy(valid_begin_, valid_end_, recv_buffer_.begin());
                valid_begin_ = recv_buffer_.begin();
            }

            socket_.async_read_some(
                boost::asio::buffer(valid_end_, recv_buffer_.end() - valid_end_),
                boost::bind(
                    &this_type::read_message<State, Handler>,
                    this->shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred,
                    message_state,
                    handler
                )
            );
        }
    }

private:
    template <typename State, typename Handler>
    void read_message_body(const boost::system::error_code& error,
                           std::size_t bytes_transfered,
                           read_destination xfer_dest,
                           State message_state,
                           Handler handler)
    {
//        if (xfer_dest == dest_recv_buffer)
        valid_end_ += bytes_transfered;

        std::vector<boost::asio::mutable_buffer> external_buffers
            = message_state->parse_body(boost::asio::const_buffer(valid_begin_, valid_end_ - valid_begin_));

        reset_recv_buffer();

        if (error || message_state->body_length_remaining() == 0) {
            handler(error);
            return;
        }

        if (external_buffers.empty()) {
            external_buffers.push_back(boost::asio::buffer(valid_end_, std::min(std::size_t(recv_buffer_.end() - valid_end_), message_state->body_length_remaining())));
            xfer_dest = dest_recv_buffer;
        }
        else {
            xfer_dest = dest_external_buffer;
        }

        boost::asio::async_read(socket_, external_buffers,
            boost::bind(&this_type::read_message_body<State, Handler>,
                        this->shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred,
                        xfer_dest,
                        message_state,
                        handler));
    }

    void reset_recv_buffer()
    {
        valid_begin_ = valid_end_ = recv_buffer_.begin();
    }

protected:
    recv_buffer_t recv_buffer_;
    typename recv_buffer_t::iterator valid_begin_, valid_end_;

    next_layer_type socket_;
    generation_state send_buffer_;
    send_queue_t send_queue_;
};

}

#endif

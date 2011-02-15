//
// async_read.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ASYNC_READ_HPP
#define HTTP_ASYNC_READ_HPP

#include <http/basic_message_parser.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/cstdint.hpp>

#include <vector>
#include <deque>

namespace http {

namespace detail {

template <typename Msg, typename Handler>
struct read_op
{
    typedef std::deque<std::vector<boost::uint8_t> > buffers_type;

    class iterator : public boost::iterator_facade<
        iterator,
        buffers_type::value_type::value_type,
        boost::forward_traversal_tag
        >
    {
    public:
        iterator(buffers_type::iterator begin, buffers_type::iterator end)
            : current_buffer_(begin), end_buffer_(end)
        {
            if (current_buffer_ != end_buffer_)
                pos_ = current_buffer->begin();
        }

        buffers_type::value_type::value_type&
        dereference()
        {
            return *pos_;
        }

        bool equal(const iterator& other)
        {
            return (current_buffer_ == other.current_buffer_
                 && current_buffer_ == end_buffer_)
                || (pos_ == other.pos_
                 && current_buffer_ == other.current_buffer_
                 && end_buffer_ == other.end_buffer_);
        }

        void increment()
        {
            ++pos_;
            if (pos_ == current_buffer_->end())
                if (++current_buffer_ != end_buffer_)
                    pos_ = current_buffer_->begin();
        }

    private:
        buffers_type::value_type::iterator pos_;
        buffers_type::iterator current_buffer_, end_buffer_;
    };

private:
    Handler handler_;
    basic_message_parser<Msg, iterator> parser_;
    buffers_type buffers_;
};

} // namespace detail

template <typename AsyncReadStream, typename Msg, typename Handler>
async_read(AsyncReadStream& s, Msg& m, Handler h)
{

}

} // namespace http

#endif

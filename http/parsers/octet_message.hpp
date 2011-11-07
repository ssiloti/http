//
// octet_message.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_PARSERS_STRING_MESSAGE_HPP
#define HTTP_PARSERS_STRING_MESSAGE_HPP

#include <http/basic_message.hpp>
#include <http/parsers/body.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/cstdint.hpp>
#include <vector>

namespace http { namespace parsers {

template <typename Headers>
class body_parser<Headers, std::vector<boost::uint8_t> >
{
public:
    std::vector<boost::asio::mutable_buffer>
    parse_body(basic_message<Headers, std::vector<boost::uint8_t> >& msg, boost::asio::const_buffer received, std::size_t remaining)
    {
        using namespace boost::asio;

        if (remaining == std::numeric_limits<std::size_t>::max())
        {
            remaining = 65536;
        }
        msg.body.reserve(msg.body.size() + remaining + (msg.body.size() ? 0 : buffer_size(received)));

        if (msg.body.size() == 0)
        {
            msg.body.insert(msg.body.end(), buffer_cast<const char*>(received), buffer_cast<const char*>(received) + buffer_size(received));
        }

        if (remaining != 0)
        {
            std::size_t valid_size = msg.body.size();
            msg.body.resize(msg.body.size() + remaining);
            return std::vector<mutable_buffer>(1, mutable_buffer(&msg.body[valid_size], msg.body.size() - valid_size));
        }
        else
            return std::vector<mutable_buffer>();
    }
};

}

}

#endif

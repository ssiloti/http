//
// string_message.hpp
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
#include <vector>
#include <string>

namespace http { namespace parsers {

template <typename Headers>
class body_parser<Headers, std::string>
{
public:
    std::vector<boost::asio::mutable_buffer>
    parse_body(basic_message<Headers, std::string>& msg, boost::asio::const_buffer received, std::size_t remaining)
    {
        using namespace boost::asio;

        msg.body.assign(buffer_cast<const char*>(received), buffer_size(received));
        return std::vector<boost::asio::mutable_buffer>();
    }
};

}

}

#endif

//
// message.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_GENERATORS_MESSAGE_HPP
#define HTTP_GENERATORS_MESSAGE_HPP

#include <boost/smart_ptr.hpp>
#include <algorithm>

namespace http { namespace generators {

template <typename OutputIterator, typename Msg>
bool generate_message(OutputIterator& sink, const Msg& msg)
{
    if (!msg.generate_start_line(sink))
        return false;

    for (typename Msg::headers_type::const_iterator header(msg.headers.begin());
        header != msg.headers.end();
        ++header)
    {
        std::copy(header->first.begin(), header->first.end(), sink);
        *sink++ = ':';
        if (!header->second.generate(sink))
            return false;
        *sink++ = '\r';
        *sink++ = '\n';
    }

    *sink++ = '\r';
    *sink++ = '\n';

    if (!generate_body(sink, msg.body))
        return false;

    return true;
}

template <typename OutputIterator, typename Msg>
bool generate_message(OutputIterator& sink, boost::shared_ptr<Msg> msg)
{
    return generate_message(sink, *msg);
}

} } // namespace generators namespace http

#endif

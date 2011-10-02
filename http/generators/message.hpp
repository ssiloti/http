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

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/typeof/typeof.hpp>
#include <algorithm>

#include <http/headers_map.hpp>

namespace http { namespace generators {

namespace detail {
    template <typename OutputIterator, typename Header>
    bool generate_header(OutputIterator sink, Header header)
    {
        std::copy(header.first.begin(), header.first.end(), sink);
        *sink++ = ':';
        if (!header.second.generate(sink))
            return false;
        *sink++ = '\r';
        *sink++ = '\n';
        return true;
    }

    template <typename Header>
    struct aux_header_wrapper
    {
        aux_header_wrapper(const Header& h) : header(h) {}

        template <typename OutputIterator>
        bool generate(OutputIterator sink)
        {
            return generators::generate_header_value(header, sink);
        }

        const Header& header;
    };

    template <typename Headers, typename OutputIterator>
    struct generate_aux_headers
    {
        generate_aux_headers(const Headers& h, OutputIterator s) : headers(h), sink(s) {}

        template <typename Aux>
        void operator()(const Aux& aux_header) const
        {
            if (headers.find(boost::mpl::c_str<typename Aux::first_type>::value) == headers.end())
            {
                generate_header(sink,
                    std::make_pair(
                        std::string(
                            boost::mpl::c_str<typename Aux::first_type>::value,
                            boost::mpl::size<typename Aux::first_type>::value
                        ),
                        aux_header_wrapper<Aux>(aux_header)
                    )
                );
                // TODO: throw on false?
            }
        }

        const Headers& headers;
        mutable OutputIterator sink;
    };
}

template <typename OutputIterator, typename Msg>
bool generate_message(OutputIterator& sink, const Msg& msg)
{
    typename OutputIterator::buffer_handle  headers_handle = sink.precommit_buffer();
    auto content_headers(generate_body(sink, msg.body));

    if (!msg.generate_start_line(sink))
        return false;

    boost::fusion::for_each(content_headers, detail::generate_aux_headers<typename Msg::headers_map_type, OutputIterator>(msg.headers, sink));

    for (typename Msg::headers_map_type::const_iterator header(msg.headers.begin());
        header != msg.headers.end();
        ++header)
    {
        if (!detail::generate_header(sink, *header))
            return false;
    }

    *sink++ = '\r';
    *sink++ = '\n';

    sink.commit_buffer(headers_handle);

    return true;
}

template <typename OutputIterator, typename Msg>
bool generate_message(OutputIterator& sink, boost::shared_ptr<Msg> msg)
{
    return generate_message(sink, *msg);
}

} } // namespace generators namespace http

#endif

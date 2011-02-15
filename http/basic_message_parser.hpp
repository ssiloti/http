//
// basic_message_parser.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_MESSAGE_PARSER_HPP
#define HTTP_BASIC_MESSAGE_PARSER_HPP

#include <boost/range.hpp>
#include <boost/logic/tribool.hpp>

#include <vector>
#include <utility>

namespace http {

template <typename Msg, typename InputIterator>
class basic_message_parser
{
    enum state
    {
        state_init,
        state_start_line,
        state_start_line_cr,
        state_header_start,
        state_header_name,
        state_header_value,
        state_header_cr,
        state_header_lf,
        state_header_final_cr,
        state_header_final_lf,
        state_body,
    };

public:
    typedef Msg message_type;
    typedef InputIterator iterator;

    basic_message_parser(message_type& m)
        : msg_(m), state_(state_init)
    {}

    boost::tribool parse(iterator& begin, iterator end)
    {
        if (state_ == state_init)
        {
            cur_ = begin;
            state_ = state_start_line;
        }

        for (;cur_ != end; ++cur_)
        {
            switch (state_)
            {
            case state_start_line:
                if (*cur_ == '\r')
                    state_ = state_start_line_cr;
                break;
            case state_start_line_cr:
                if (*cur_ == '\n') {
                    iterator parse_end = cur_;
                    msg_.parse_start_line(begin, --parse_end);
                    begin = cur_; ++begin;
                    state_ = state_header_start;
                }
                else {
                    return false;
                }
                break;
            case state_header_start:
                if (*cur_ == '\r')
                    state_ = state_header_final_cr;
                else
                    state_ = state_header_name;
                break;
            case state_header_name:
                if (*cur_ == ':') {
                    // TODO: Save location for header parse call?
                    state_ = state_header_value;
                }
                break;
            case state_header_value:
                if (*cur_ == '\r')
                    state_ = state_header_cr;
                break;
            case state_header_cr:
                if (*cur_ == '\n')
                    state_ = state_header_lf;
                else
                    return false;
                break;
            case state_header_lf:
                if (*cur_ == ' ' || *cur_ == '\t')
                    state_ = state_header_value;
                else {
                    iterator parse_end = cur_;
                    --parse_end; --parse_end;
                    msg_.parse_header(begin, parse_end);
                    begin = cur_;
                    state_ = state_header_name;
                }
                break;
            case state_header_final_cr:
                if (*cur_ == '\n')
                    state_ = state_body;
                else
                    return false;
                break;
            default:
                return false;
                break;
            }
        }
        return boost::indeterminate;
    }


private:
    message_type& msg_;
    iterator cur_;
    state state_;
};

} // namespace http

#endif

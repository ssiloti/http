//
// basic_response.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_RESPONSE_HPP
#define HTTP_BASIC_RESPONSE_HPP

#include <http/basic_message.hpp>
#include <http/status_code.hpp>

#include <boost/fusion/tuple.hpp>

#include <string>

namespace http {

template <typename Headers, typename Body>
class basic_response : public basic_message<Headers, Body>
{
private:
    typedef boost::fusion::tuple<
        typename basic_message<Headers, Body>::version_number_type&,
        status_code&,
        std::string&
    > tuple_type;

    template <typename Iterator>
    struct status_line_grammar;

    template <typename Iterator>
    friend struct status_line_grammar;

public:
    basic_response()
        : status(status_ok), reason("OK")
    {}

    template <typename InputIterator>
    bool parse_start_line(InputIterator begin, InputIterator end);

    template <typename OutputIterator>
    bool generate_start_line(OutputIterator sink) const;

    status_code status;
    std::string reason;
};

}

#endif

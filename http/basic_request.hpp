//
// basic_request.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_REQUEST_HPP
#define HTTP_BASIC_REQUEST_HPP

#include <http/basic_message.hpp>

#include <uri/basic_uri.hpp>

#include <boost/fusion/tuple.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include <boost/spirit/home/karma/domain.hpp>

namespace http {

struct asterisk
{
    asterisk() : c('*') {}
    char c;
};

template <typename Headers, typename Body>
class basic_request : public basic_message<Headers, Body>
{
public:
    typedef boost::variant<
        asterisk
        , uri::basic_uri<std::string>
        , boost::fusion::tuple<std::string, boost::optional<std::string> >
        , uri::basic_authority<std::string>
    > target_type;

private:
    typedef boost::fusion::tuple<
        std::string&,
        target_type&,
        typename basic_message<Headers, Body>::version_number_type&
    > tuple_type;

    template <typename Iterator>
    friend struct request_line_grammar;

    template <typename Iterator>
    struct request_line_grammar;

public:
    template <typename InputIterator>
    bool parse_start_line(InputIterator begin, InputIterator end);

    template <typename OutputIterator>
    bool generate_start_line(OutputIterator sink) const;

    std::map<std::string, std::string> parse_query() const;

    void clear()
    {
        basic_message<Headers, Body>::clear();
        method.clear();
        target = asterisk();
    }

    std::string method;
    target_type target;
};

}

namespace boost { namespace spirit { namespace traits {

template <>
struct transform_attribute<
    const http::asterisk,
    char,
    boost::spirit::karma::domain
>
{
    typedef char type;

    static type pre(const http::asterisk& exposed) {
        return type(exposed.c);
    }
};

} } }


#endif

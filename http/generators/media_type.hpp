//
// headers.hpp
// ~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_GENERATORS_MEDIA_TYPE_HPP
#define HTTP_GENERATORS_MEDIA_TYPE_HPP

#include <http/media_type.hpp>

#include <boost/spirit/home/karma/char.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>

#include <boost/spirit/home/karma/nonterminal/rule.hpp>
#include <boost/spirit/home/karma/nonterminal/grammar.hpp>

#include <boost/fusion/include/std_pair.hpp>

namespace http { namespace generators {

template <typename Iterator>
struct media_type
    : boost::spirit::karma::grammar<Iterator, http::media_type()>
{
    media_type() : media_type::base_type(start, "media_type")
    {
        using namespace boost::spirit::karma;

        start = ascii::string << '/' << ascii::string << parameters;
        parameters = *(';' << parameter);
        parameter = ascii::string << '=' << ascii::string;
    }

    // start rule of grammar
    boost::spirit::karma::rule<Iterator, http::media_type()> start;

    boost::spirit::karma::rule<Iterator, std::map<std::string, std::string>()> parameters;
    boost::spirit::karma::rule<Iterator, std::pair<std::string, std::string>()> parameter;
};

} }

#endif

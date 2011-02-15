//
// basic_parse.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <uri/grammar.hpp>

#include <boost/spirit/home/qi/parse.hpp>

#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;

int main()
{
    uri::basic_uri<std::string> test_uri;
    std::string test_string("http://example.com:8080");
    std::string::iterator begin(test_string.begin());

    std::cout << boost::spirit::qi::parse(begin, test_string.end(), uri::grammar<std::string::iterator, std::string>(), test_uri) << std::endl;
    std::cout << test_uri.authority->port.get();
}

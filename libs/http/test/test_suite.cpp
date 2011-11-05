//
// test_suite.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/test/unit_test.hpp>

void init_basic_request_suite(int, char*[]);
void init_basic_response_suite(int, char*[]);
void init_basic_message_parser_suite(int, char*[]);
void init_headers_suite(int, char*[]);

boost::unit_test::test_suite* init_unit_test_suite(int argc, char*argv[])
{
    init_basic_request_suite(argc, argv);
    init_basic_response_suite(argc, argv);
    init_basic_message_parser_suite(argc, argv);
    init_headers_suite(argc, argv);
    return NULL;
}

//
// headers.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <http/parsers/headers.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

using boost::unit_test::test_suite;
using namespace http;

void test_header_parsers()
{
    {
        headers::cache_control cc;
        std::string testv_pass("token, extension-token = token-param, extension-token-q = \"qouted param\"");
        BOOST_CHECK(parsers::parse_header(cc, testv_pass.begin(), testv_pass.end()));
        BOOST_CHECK(cc.second.count("token"));
        BOOST_CHECK_EQUAL(cc.second["token"], std::string());
        BOOST_CHECK(cc.second.count("extension-token"));
        BOOST_CHECK_EQUAL(cc.second["extension-token"], std::string("token-param"));
        BOOST_CHECK(cc.second.count("extension-token-q"));
        BOOST_CHECK_EQUAL(cc.second["extension-token-q"], std::string("qouted param"));
    }

    {
        headers::content_length cl;
        std::string testv_pass("1234");
        std::string testv_fail("-1");

        BOOST_CHECK(parsers::parse_header(cl, testv_pass.begin(), testv_pass.end()));
        BOOST_CHECK_EQUAL(cl.second, 1234);
        BOOST_CHECK(!parsers::parse_header(cl, testv_fail.begin(), testv_fail.end()));
    }
}

test_suite* init_unit_test_suite(int, char*[])
{
  test_suite* test = BOOST_TEST_SUITE("basic_message_parser");
  test->add(BOOST_TEST_CASE(&test_header_parsers));
  return test;
}

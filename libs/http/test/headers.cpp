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

#include <algorithm>

using boost::unit_test::test_suite;
using namespace http;

template <typename Header>
void test_token_map()
{
    Header h;
    std::string testv_pass("token, extension-token = token-param, extension-token-q = \"quoted param\"");
    BOOST_REQUIRE(parsers::parse_header(h, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK(h.second.count("token"));
    BOOST_CHECK_EQUAL(h.second["token"], std::string());
    BOOST_CHECK(h.second.count("extension-token"));
    BOOST_CHECK_EQUAL(h.second["extension-token"], std::string("token-param"));
    BOOST_CHECK(h.second.count("extension-token-q"));
    BOOST_CHECK_EQUAL(h.second["extension-token-q"], std::string("quoted param"));
}

template <typename Header>
void test_token_vector()
{
    Header h;
    std::string testv_pass("token1, token2, token3");
    BOOST_REQUIRE(parsers::parse_header(h, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(h.second.size(), 3);
    BOOST_CHECK(std::find(h.second.begin(), h.second.end(), std::string("token1")) != h.second.end());
    BOOST_CHECK(std::find(h.second.begin(), h.second.end(), std::string("token2")) != h.second.end());
    BOOST_CHECK(std::find(h.second.begin(), h.second.end(), std::string("token3")) != h.second.end());
}

void test_date()
{
    headers::date d;
    std::string testv_pass("Tue, 15 Nov 1994 08:12:31 GMT");
    BOOST_REQUIRE(parsers::parse_header(d, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(d.second.date().day(), 15);
    BOOST_CHECK_EQUAL(d.second.date().month(), boost::gregorian::Nov);
    BOOST_CHECK_EQUAL(d.second.date().year(), 1994);
    BOOST_CHECK_EQUAL(d.second.time_of_day().hours(), 8);
    BOOST_CHECK_EQUAL(d.second.time_of_day().minutes(), 12);
    BOOST_CHECK_EQUAL(d.second.time_of_day().seconds(), 31);
}

void test_content_length()
{
    headers::content_length cl;
    std::string testv_pass("1234");
    std::string testv_fail("-1");

    BOOST_REQUIRE(parsers::parse_header(cl, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(cl.second, 1234);
    BOOST_CHECK(!parsers::parse_header(cl, testv_fail.begin(), testv_fail.end()));
}

test_suite* init_unit_test_suite(int, char*[])
{
  test_suite* test = BOOST_TEST_SUITE("headers");
  test->add(BOOST_TEST_CASE(&test_token_map<headers::cache_control>));
  test->add(BOOST_TEST_CASE(&test_token_map<headers::pragma>));
  test->add(BOOST_TEST_CASE(&test_token_vector<headers::connection>));
  test->add(BOOST_TEST_CASE(&test_token_vector<headers::trailer>));
  test->add(BOOST_TEST_CASE(&test_token_vector<headers::allow>));
  test->add(BOOST_TEST_CASE(&test_date));
  test->add(BOOST_TEST_CASE(&test_content_length));
  return test;
}

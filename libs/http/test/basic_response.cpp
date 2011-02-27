//
// basic_response.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <http/basic_response.hpp>
#include <http/parsers/response.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#include <boost/mpl/map/map0.hpp>

using boost::unit_test::test_suite;

void test_response()
{
    http::basic_response<boost::mpl::map0<>, std::string> response;

    std::string status_line("HTTP/1.1 200 OK");
    std::string::iterator begin(status_line.begin());

    BOOST_CHECK(response.parse_start_line(begin, status_line.end()));

    BOOST_CHECK_EQUAL(response.version.major, 1);
    BOOST_CHECK_EQUAL(response.version.minor, 1);
    BOOST_CHECK_EQUAL(response.status, http::status_ok);
    BOOST_CHECK_EQUAL(response.reason, std::string("OK"));
}

test_suite* init_unit_test_suite(int, char*[])
{
  test_suite* test = BOOST_TEST_SUITE("basic_response");
  test->add(BOOST_TEST_CASE(&test_response));
  return test;
}

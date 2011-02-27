//
// basic_message_parser.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <http/parsers/message_state.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#include <string>
#include <vector>

using boost::unit_test::test_suite;

struct test_message
{
    test_message(const std::string& start_line, const std::vector<std::string>& headers)
        : start_line_(start_line), headers_(headers)
    {
        current_header_ = headers_.begin();
    }

    template <typename InputIterator>
    bool parse_start_line(InputIterator begin, InputIterator end)
    {
        BOOST_CHECK_MESSAGE(std::equal(begin, end, start_line_.begin()),
                            "Start line was parsed as " << std::string(begin, end) << ", expected " << start_line_);
        return true;
    }

    template <typename InputIterator>
    bool parse_header(InputIterator begin, InputIterator sep, InputIterator end)
    {
        BOOST_CHECK_MESSAGE(std::equal(begin, end,
                               current_header_->begin()),
                            "Header parsed as " << std::string(begin, end) << ", expected " << *current_header_);
        ++current_header_;
        return true;
    }

    std::string start_line_;
    std::vector<std::string> headers_;
    std::vector<std::string>::iterator current_header_;
};

void test_message_parser()
{
    const std::string test_string("GET index.html HTTP/1.1\r\nAccept: text/html\r\nAllow: text/html\r\n\r\n");
    std::vector<std::string> headers;
    headers.push_back("Accept: text/html");
    headers.push_back("Allow: text/html");

    {
        test_message test_msg("GET index.html HTTP/1.1", headers);
        http::parsers::message_state<test_message, std::string::const_iterator> test_parser(test_msg);
    
        std::string::const_iterator begin = test_string.begin();
        test_parser.parse(begin, test_string.end());
    }

    {
        test_message test_msg("GET index.html HTTP/1.1", headers);
        http::parsers::message_state<test_message, std::string::const_iterator> test_parser(test_msg);

        std::string::const_iterator begin = test_string.begin();
        std::string::const_iterator end = begin + 1;
        for (; end != test_string.end(); ++end)
        {
            test_parser.parse(begin, end);
        }
    }

    {
        test_message test_msg("GET index.html HTTP/1.1", headers);
        http::parsers::message_state<test_message, std::string::const_iterator> test_parser(test_msg);

        std::string::const_iterator begin = test_string.begin();
        std::string::const_iterator end = begin + 2;
        for (; end != test_string.end(); end += 2)
        {
            test_parser.parse(begin, end);
        }
    }
}

test_suite* init_unit_test_suite(int, char*[])
{
  test_suite* test = BOOST_TEST_SUITE("basic_message_parser");
  test->add(BOOST_TEST_CASE(&test_message_parser));
  return test;
}

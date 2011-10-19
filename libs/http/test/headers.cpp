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

void test_transfer_encoding()
{
    headers::transfer_encoding te;
    std::string testv_pass("token1, token2; param1=word, token3");
    BOOST_REQUIRE(parsers::parse_header(te, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(te.second.size(), 3);
    BOOST_CHECK(te.second.find("token1") != te.second.end());
    BOOST_CHECK(te.second.find("token2") != te.second.end());
    BOOST_CHECK(te.second.find("token3") != te.second.end());
    BOOST_CHECK(te.second["token2"].find("param1") != te.second["token2"].end());
    BOOST_CHECK_EQUAL(te.second["token2"]["param1"], std::string("word"));
}

void test_upgrade()
{
    headers::upgrade u;
    std::string testv_pass("product1, product2/version2, product3");
    BOOST_REQUIRE(parsers::parse_header(u, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(u.second.size(), 3);
    BOOST_CHECK_EQUAL(u.second[0].product, std::string("product1"));
    BOOST_CHECK_EQUAL(u.second[0].version.size(), 0);
    BOOST_CHECK_EQUAL(u.second[1].product, std::string("product2"));
    BOOST_CHECK_EQUAL(u.second[1].version, std::string("version2"));
    BOOST_CHECK_EQUAL(u.second[2].product, std::string("product3"));
    BOOST_CHECK_EQUAL(u.second[2].version.size(), 0);
}

void test_via()
{
    headers::via v;
    std::string testv_pass("1.0 fred, 1.1 p.example.net (Apache/1.1)");
    BOOST_REQUIRE(parsers::parse_header(v, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(v.second.size(), 2);
    BOOST_CHECK_EQUAL(v.second[0].received_protocol.version, std::string("1.0"));
    BOOST_CHECK_EQUAL(v.second[0].received_by.host, std::string("fred"));
    BOOST_CHECK_EQUAL(v.second[0].comment.size(), 0);
    BOOST_CHECK_EQUAL(v.second[1].received_protocol.version, std::string("1.1"));
    BOOST_CHECK_EQUAL(v.second[1].received_by.host, std::string("p.example.net"));
    BOOST_CHECK_EQUAL(v.second[1].comment.size(), 1);
    BOOST_CHECK(v.second[1].comment[0].type() == typeid(std::string));
    BOOST_CHECK_EQUAL(boost::get<std::string>(v.second[1].comment[0]), std::string("Apache/1.1"));
}

void test_warning()
{
    headers::warning w;
    std::string testv_pass("123 p.example.net:1 \"danger\", 111 t.example.net \"will\" \"Tue, 15 Nov 1994 08:12:31 GMT\"");
    BOOST_REQUIRE(parsers::parse_header(w, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(w.second.size(), 2);
    BOOST_CHECK_EQUAL(w.second[0].code, 123);
    BOOST_CHECK_EQUAL(w.second[0].agent.host, std::string("p.example.net"));
    BOOST_CHECK_EQUAL(w.second[0].agent.port.get(), 1);
    BOOST_CHECK_EQUAL(w.second[0].text, std::string("danger"));
    BOOST_CHECK(!w.second[0].date);
    BOOST_CHECK_EQUAL(w.second[1].code, 111);
    BOOST_CHECK_EQUAL(w.second[1].agent.host, std::string("t.example.net"));
    BOOST_CHECK(!w.second[1].agent.port);
    BOOST_CHECK_EQUAL(w.second[1].text, std::string("will"));
    BOOST_CHECK_EQUAL(w.second[1].date->date().day(), 15);
    BOOST_CHECK_EQUAL(w.second[1].date->date().month(), boost::gregorian::Nov);
    BOOST_CHECK_EQUAL(w.second[1].date->date().year(), 1994);
    BOOST_CHECK_EQUAL(w.second[1].date->time_of_day().hours(), 8);
    BOOST_CHECK_EQUAL(w.second[1].date->time_of_day().minutes(), 12);
    BOOST_CHECK_EQUAL(w.second[1].date->time_of_day().seconds(), 31);
}

void test_accept()
{
    headers::accept a;
    std::string testv_pass("audio/*; q=0.2, audio/basic");
    BOOST_REQUIRE(parsers::parse_header(a, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(a.second.size(), 2);
    BOOST_CHECK_EQUAL(a.second[0].media_range.type, std::string("audio"));
    BOOST_CHECK_EQUAL(a.second[0].media_range.subtype, std::string("*"));
    BOOST_CHECK_EQUAL(a.second[0].params.get().qvalue, 0.2f);
    BOOST_CHECK_EQUAL(a.second[1].media_range.type, std::string("audio"));
    BOOST_CHECK_EQUAL(a.second[1].media_range.subtype, std::string("basic"));
    BOOST_CHECK(!a.second[1].params);
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

void test_content_type()
{
    headers::content_type ct;
    std::string testv_pass("text/html; charset=ISO-8859-4");
    BOOST_REQUIRE(parsers::parse_header(ct, testv_pass.begin(), testv_pass.end()));
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
    test->add(BOOST_TEST_CASE(&test_transfer_encoding));
    test->add(BOOST_TEST_CASE(&test_upgrade));
    test->add(BOOST_TEST_CASE(&test_via));
    test->add(BOOST_TEST_CASE(&test_warning));
    test->add(BOOST_TEST_CASE(&test_accept));
    test->add(BOOST_TEST_CASE(&test_content_length));
    test->add(BOOST_TEST_CASE(&test_content_type));
    return test;
}

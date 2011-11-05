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
#include <algorithm>

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

void test_cache_control()
{
    test_token_map<headers::cache_control>();
}

void test_pragma()
{
    test_token_map<headers::pragma>();
}

void test_connection()
{
    test_token_vector<headers::connection>();
}

void test_trailer()
{
    test_token_vector<headers::trailer>();
}

void test_allow()
{
    test_token_vector<headers::allow>();
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

void test_accept_charset()
{
    headers::accept_charset ac;
    std::string testv_pass("iso-8859-5, unicode-1-1;q=0.8");
    BOOST_REQUIRE(parsers::parse_header(ac, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(ac.second.size(), 2);
    BOOST_CHECK_EQUAL(ac.second[0].charset, std::string("iso-8859-5"));
    BOOST_CHECK(!ac.second[0].qvalue);
    BOOST_CHECK_EQUAL(ac.second[1].charset, std::string("unicode-1-1"));
    BOOST_CHECK_EQUAL(ac.second[1].qvalue.get(), 0.8f);
}

void test_accept_encoding()
{
    headers::accept_encoding ae;
    std::string testv_pass("compress, gzip;q=1.0");
    BOOST_REQUIRE(parsers::parse_header(ae, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(ae.second.size(), 2);
    BOOST_CHECK_EQUAL(ae.second[0].coding, std::string("compress"));
    BOOST_CHECK(!ae.second[0].qvalue);
    BOOST_CHECK_EQUAL(ae.second[1].coding, std::string("gzip"));
    BOOST_CHECK_EQUAL(ae.second[1].qvalue.get(), 1.0f);
}

void test_accept_language()
{
    headers::accept_language al;
    std::string testv_pass("da, en-gb;q=0.8, en;q=0.7");
    BOOST_REQUIRE(parsers::parse_header(al, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(al.second.size(), 3);
    BOOST_CHECK_EQUAL(al.second[0].range, std::string("da"));
    BOOST_CHECK(!al.second[0].qvalue);
    BOOST_CHECK_EQUAL(al.second[1].range, std::string("en-gb"));
    BOOST_CHECK_EQUAL(al.second[1].qvalue.get(), 0.8f);
    BOOST_CHECK_EQUAL(al.second[2].range, std::string("en"));
    BOOST_CHECK_EQUAL(al.second[2].qvalue.get(), 0.7f);
}

void test_authorization()
{
    headers::authorization a;
    std::string testv_pass("Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    BOOST_REQUIRE(parsers::parse_header(a, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(a.second.scheme, std::string("Basic"));
    BOOST_CHECK_EQUAL(boost::get<std::string>(a.second.params.get()), std::string("QWxhZGRpbjpvcGVuIHNlc2FtZQ=="));
}

void test_expect()
{
    headers::expect e;
    std::string testv_pass("100-continue, extention = value; param=value");
    BOOST_REQUIRE(parsers::parse_header(e, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(e.second.size(), 2);
    BOOST_CHECK_EQUAL(e.second.count("100-continue"), 1);
    BOOST_CHECK_EQUAL(boost::fusion::at_c<0>(e.second["extention"]), std::string("value"));
    BOOST_CHECK_EQUAL(boost::fusion::at_c<1>(e.second["extention"]).size(), 1);
    BOOST_CHECK_EQUAL(boost::fusion::at_c<1>(e.second["extention"])["param"], std::string("value"));
}

void test_host()
{
    headers::host h;
    std::string testv_pass("www.example.org");
    BOOST_REQUIRE(parsers::parse_header(h, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(h.second.host, std::string("www.example.org"));
}

void test_if_match()
{
    headers::if_match ifm;
    std::string testv_pass("W/\"xyzzy\", \"r2d2xxxx\", \"c3piozzzz\"");
    BOOST_REQUIRE(parsers::parse_header(ifm, testv_pass.begin(), testv_pass.end()));
    BOOST_CHECK_EQUAL(boost::get<std::vector<http::entity_tag> >(ifm.second)[0].weak, true);
    BOOST_CHECK_EQUAL(boost::get<std::vector<http::entity_tag> >(ifm.second)[0].tag, std::string("xyzzy"));
    BOOST_CHECK_EQUAL(boost::get<std::vector<http::entity_tag> >(ifm.second)[1].weak, false);
    BOOST_CHECK_EQUAL(boost::get<std::vector<http::entity_tag> >(ifm.second)[1].tag, std::string("r2d2xxxx"));
    BOOST_CHECK_EQUAL(boost::get<std::vector<http::entity_tag> >(ifm.second)[2].weak, false);
    BOOST_CHECK_EQUAL(boost::get<std::vector<http::entity_tag> >(ifm.second)[2].tag, std::string("c3piozzzz"));
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

void init_headers_suite(int, char*[])
{
    boost::unit_test::test_suite* test = BOOST_TEST_SUITE("headers");
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
    test->add(BOOST_TEST_CASE(&test_accept_charset));
    test->add(BOOST_TEST_CASE(&test_accept_encoding));
    test->add(BOOST_TEST_CASE(&test_accept_language));
    test->add(BOOST_TEST_CASE(&test_authorization));
    test->add(BOOST_TEST_CASE(&test_expect));
    test->add(BOOST_TEST_CASE(&test_host));
    test->add(BOOST_TEST_CASE(&test_if_match));
    test->add(BOOST_TEST_CASE(&test_content_length));
    test->add(BOOST_TEST_CASE(&test_content_type));

    boost::unit_test::framework::master_test_suite().add(test);
}

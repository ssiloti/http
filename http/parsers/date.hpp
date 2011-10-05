//
// date.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_PARSERS_DATE_HPP
#define HTTP_PARSERS_DATE_HPP

#include <boost/spirit/home/qi/string/symbols.hpp>
#include <boost/spirit/home/qi/char/char.hpp>
#include <boost/spirit/home/qi/operator.hpp>
#include <boost/spirit/home/qi/numeric.hpp>
#include <boost/spirit/home/qi/string.hpp>
#include <boost/spirit/home/qi/nonterminal/grammar.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace http { namespace parsers {

namespace detail {

typedef boost::fusion::tuple<
    // day_name
    //std::string,
    // day. month, year
    boost::fusion::tuple<unsigned, boost::date_time::months_of_year, unsigned>,
    // hour, minute, second
    boost::fusion::tuple<unsigned, unsigned, unsigned>
    > date_tuple_type;

struct month_parser : boost::spirit::qi::symbols<char, boost::date_time::months_of_year>
{
    month_parser()
    {
        using namespace boost::date_time;

        add
            ("Jan", Jan)
            ("Feb", Feb)
            ("Mar", Mar)
            ("Apr", Apr)
            ("May", May)
            ("Jun", Jun)
            ("Jul", Jul)
            ("Aug", Aug)
            ("Sep", Sep)
            ("Oct", Oct)
            ("Nov", Nov)
            ("Dec", Dec);
    }
};

} } }

namespace boost { namespace spirit { namespace traits {

template <>
struct transform_attribute<
    boost::posix_time::ptime,
    http::parsers::detail::date_tuple_type,
    boost::spirit::qi::domain
>
{
    typedef http::parsers::detail::date_tuple_type type;

    static type pre(boost::posix_time::ptime& exposed)
    {
        return type();
    }

    static void post(boost::posix_time::ptime& exposed, type const& attr)
    {
        exposed = boost::posix_time::ptime(
            boost::gregorian::date(get<2>(get<0>(attr)), get<1>(get<0>(attr)), get<0>(get<0>(attr))),
            boost::posix_time::time_duration(get<0>(get<1>(attr)), get<1>(get<1>(attr)), get<2>(get<1>(attr)), 0));
    }

    static void fail(boost::posix_time::ptime& val) { }
};

} } }

namespace http { namespace parsers {

template <typename Iterator>
struct date_grammar : boost::spirit::qi::grammar<Iterator, boost::posix_time::ptime()>
{
    date_grammar() : date_grammar::base_type(http_date)
    {
        using namespace boost::spirit::qi;

        http_date %= rfc1123_date.alias();
        rfc1123_date %= day_name >> ',' >> ' ' >> date1 >> ' ' >> time_of_day >> " GMT";
        day_name %= lit("Mon") | lit("Tue") | lit("Wed") | lit("Thu") | lit("Fri") | lit("Sat") | lit("Sun");
        date1 %= uint_parser<unsigned, 10, 2, 2>() >> ' ' >> month >> ' ' >> uint_parser<unsigned, 10, 4, 4>();
        time_of_day %= uint_parser<unsigned, 10, 2, 2>() >> ':' >> uint_parser<unsigned, 10, 2, 2>() >> ':' >> uint_parser<unsigned, 10, 2, 2>();
    }

    boost::spirit::qi::rule<Iterator> day_name;
    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<unsigned, boost::date_time::months_of_year, unsigned>()> date1;
    detail::month_parser month;
    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<unsigned, unsigned, unsigned>()> time_of_day;

    // TODO: Add obsolete date formats (RFC850, asctime())

    boost::spirit::qi::rule<Iterator, detail::date_tuple_type()>  rfc1123_date;
    boost::spirit::qi::rule<Iterator, boost::posix_time::ptime()> http_date;
};

} }

#endif

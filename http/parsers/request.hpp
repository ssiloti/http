//
// request.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_PARSERS_REQUEST_HPP
#define HTTP_PARSERS_REQUEST_HPP

#include <http/basic_request.hpp>
#include <http/parsers/basic_rules.hpp>
#include <http/parsers/version_number.hpp>

#include <uri/parsers/components.hpp>
#include <uri/parsers/absolute_uri.hpp>

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/qi/directive/omit.hpp>
#include <boost/fusion/include/std_pair.hpp>

namespace http {

template <typename Headers, typename Body>
template <typename Iterator>
struct basic_request<Headers, Body>::request_line_grammar
    : boost::spirit::qi::grammar<Iterator, tuple_type()>
{
    request_line_grammar() : request_line_grammar::base_type(request_line)
    {
        using namespace boost::spirit;

        request_line %= method >> qi::omit[b.sp] >> request_target >> qi::omit[b.sp] >> version_number_g;
        method %= b.token;
        abs_path %= path_absolute >> -(qi::lit('?') >> query);
        request_target %= (qi::lit('*') >> qi::attr(asterisk())) | absolute_uri | abs_path | authority;
    }

    parsers::basic_rules<Iterator> b;

    uri::parsers::absolute_uri<Iterator, std::string> absolute_uri;

    uri::parsers::path_absolute<Iterator, std::string> path_absolute;
    uri::parsers::query<Iterator, std::string> query;

    uri::parsers::authority<Iterator, std::string> authority;

    parsers::version_number_grammar<Iterator> version_number_g;

    boost::spirit::qi::rule<Iterator, tuple_type()> request_line;
    boost::spirit::qi::rule<Iterator, std::string()> method;
    boost::spirit::qi::rule<Iterator, boost::fusion::tuple<std::string, boost::optional<std::string> >()> abs_path;
    boost::spirit::qi::rule<Iterator, target_type()> request_target;
};

template <typename Headers, typename Body>
template <typename InputIterator>
bool basic_request<Headers, Body>::parse_start_line(InputIterator begin, InputIterator end)
{
    tuple_type request_line_tuple(method, target, this->version);
    return boost::spirit::qi::parse(
        begin,
        end,
        request_line_grammar<InputIterator>(),
        request_line_tuple
    );
}

namespace detail
{
    template <typename Iterator>
    struct query_grammar : public boost::spirit::qi::grammar<Iterator, std::map<std::string, std::string>()>
    {
        query_grammar() : query_grammar::base_type(query_string)
        {
            using namespace boost::spirit;

            query_string = (+qchar >> -(qi::lit('=') >> +qchar)) % qi::lit("&:");
            qchar = ~qi::char_("&:=");
        }

        boost::spirit::qi::rule<Iterator, std::map<std::string, std::string>()> query_string;
        boost::spirit::qi::rule<Iterator, std::string::value_type()> qchar;
    };

    struct query_parse_visitor : public boost::static_visitor<std::map<std::string, std::string> >
    {
        std::map<std::string, std::string> operator()(const uri::basic_uri<std::string>& uri) const
        {
            if (uri.query)
                return parse(uri.query.get());
            else
                return std::map<std::string, std::string>();
        }

        std::map<std::string, std::string> operator()(const boost::fusion::tuple<std::string, boost::optional<std::string> >& uri) const
        {
            if (boost::fusion::at_c<1>(uri))
                return parse(boost::fusion::at_c<1>(uri).get());
            else
                return std::map<std::string, std::string>();
        }

        template <typename Target>
        std::map<std::string, std::string> operator()(Target t) const
        {
            return std::map<std::string, std::string>();
        }

        std::map<std::string, std::string> parse(const std::string& str) const
        {
            std::map<std::string, std::string> ret;
            std::string::const_iterator begin = str.begin();
            boost::spirit::qi::parse(begin, str.end(), query_grammar<std::string::const_iterator>(), ret);
            return ret;
        }
    };
}

template <typename Headers, typename Body>
std::map<std::string, std::string> basic_request<Headers, Body>::parse_query() const
{
    return boost::apply_visitor(detail::query_parse_visitor(), target);
}

} // namespace http

#endif

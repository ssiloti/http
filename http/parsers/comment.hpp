//
// comment.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_PARSERS_COMMENT_HPP
#define HTTP_PARSERS_COMMENT_HPP

#include <http/comment.hpp>
#include <http/parsers/basic_rules.hpp>

namespace http { namespace parsers {

template <typename Iterator>
struct comment
    : boost::spirit::qi::grammar<Iterator, http::comment()>
{
    comment() : comment::base_type(start, "comment")
    {
        start %= '(' >> *(ctext_or_quoted_cpair | start) >> ')';
        // Use a separate rule so we can adapt the std::vector<char> into an std::string
        // Qi can't adapt variant<std::vector<char>, B> to variant<std::string, B>
        ctext_or_quoted_cpair %= +(ctext | quoted_cpair);
        quoted_cpair %= '\\' >> (b.wsp | b.vchar | b.obs_text);
        // Switched from ows to (-obs_fold >> wsp) to avoid sythesizing
        // a vector attribute. ctext_or_quoted_cpair takes care of the empty case.
        ctext %= (-b.obs_fold >> b.wsp) | (b.vchar - '(' - ')' - '\\') | b.obs_text;
    }

    basic_rules<Iterator> b;

    // Defined in ietf-httpbis-p1-messaging-12, Section 3.2
    boost::spirit::qi::rule<Iterator, http::comment()> start;
    // ctext_or_quoted_cpair is non-standard, it is needed to synthesize the
    // the desired attribute from comment
    boost::spirit::qi::rule<Iterator, std::string()> ctext_or_quoted_cpair;
    boost::spirit::qi::rule<Iterator, char()> quoted_cpair;
    boost::spirit::qi::rule<Iterator, char()> ctext;
};

} }

#endif

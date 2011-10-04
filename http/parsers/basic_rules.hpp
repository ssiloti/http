//
// basic_rules.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_RULES_HPP
#define HTTP_BASIC_RULES_HPP

#include <abnf/core_rules.hpp>

#include <boost/spirit/home/qi/nonterminal/grammar.hpp>
#include <boost/spirit/home/qi/directive/lexeme.hpp>
#include <boost/spirit/home/qi/binary.hpp>
#include <boost/spirit/home/qi/char.hpp>
#include <boost/spirit/home/qi/operator.hpp>

#include <boost/variant.hpp>

#include <string>
#include <vector>

namespace http { namespace parsers {

// The basic parse rules used by HTTP 1.1
template <typename Iterator>
class basic_rules : public abnf::core_rules<Iterator>
{
public:
    typedef Iterator iterator;

    struct skipper_type : boost::spirit::qi::grammar<iterator>
    {
        skipper_type() : skipper_type::base_type(lws)
        {
            using namespace boost::spirit;

            lws = -(ascii::char_('\r') >> ascii::char_('\n')) >> +(ascii::char_(' ') | ascii::char_('\t'));
        }

        boost::spirit::qi::rule<iterator> lws;
    };

    basic_rules()
    {
        using namespace boost::spirit;

        ows = *(-obs_fold >> this->wsp);
        rws = +(-obs_fold >> this->wsp);
        bws = ows;
        obs_fold = this->crlf;

        special %= ascii::char_("()<>@,;:\\/[]?={}") | this->dquote;
        tchar   %= this->vchar - special;

        quoted_string %= lexeme[lit('"') >> *(qdtext | quoted_pair) >> lit('"')];
        token         %= lexeme[+tchar];
        word          %= token | quoted_string;
        // Switched from ows to (-obs_fold >> wsp) to avoid sythesizing
        // a vector attribute. quoted_string takes care of the empty case.
        qdtext        %= (-obs_fold >> this->wsp) | (this->vchar - this->dquote - '\\') | obs_text;
        obs_text      %= ascii::char_("\x80-\xFF");
        quoted_pair   %= lexeme['\\' >> (this->wsp | this->vchar | obs_text)];
    }

    skipper_type skipper;

    // Defined in ietf-httpbis-p1-messaging-12, Section 1.2.2
    boost::spirit::qi::rule<iterator> ows;            // "optional" whitespace
    boost::spirit::qi::rule<iterator> rws;            // "required" whitespace
    boost::spirit::qi::rule<iterator> bws;            // "bad" whitespace
    boost::spirit::qi::rule<iterator> obs_fold;       // depricated line folding

    boost::spirit::qi::rule<iterator, char()> special;
    boost::spirit::qi::rule<iterator, char()> tchar;

    boost::spirit::qi::rule<iterator, std::string()> quoted_string;
    boost::spirit::qi::rule<iterator, std::string()> token;
    boost::spirit::qi::rule<iterator, std::string()> word;
    boost::spirit::qi::rule<iterator, char()> qdtext;
    boost::spirit::qi::rule<iterator, char()> obs_text;
    boost::spirit::qi::rule<iterator, char()> quoted_pair;
};

} } // namespace parsers namespace http

#endif

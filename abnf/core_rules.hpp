//
// core_rules.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef ABNF_CORE_RULES_HPP
#define ABNF_CORE_RULES_HPP

#include <boost/spirit/home/qi/char/char_class.hpp>
#include <boost/spirit/home/qi/char/char.hpp>
#include <boost/spirit/home/qi/binary.hpp>

#include <boost/spirit/home/qi/operator.hpp>

#include <boost/spirit/home/qi/nonterminal/rule.hpp>

namespace abnf {

// ABNF rules defined in RFC 5234, Appendix B.1
template <typename Iterator>
class core_rules
{
public:
    core_rules()
    {
        using namespace boost::spirit;

        cr      = qi::lit('\r');
        bit    %= ascii::char_('0') | ascii::char_('1');
        char_   = ascii::char_("\x01-\x7F");
        crlf    = cr >> lf;
        dquote %= ascii::char_('"');
        htab   %= ascii::char_('\t');
        lf      = qi::lit('\n');
        lwsp    = *(wsp | crlf >> wsp);
        sp     %= ascii::char_(' ');
        vchar  %= ascii::char_("\x21-\x7E");
        wsp    %= sp | htab;
    }

    boost::spirit::qi::ascii::alpha_type alpha;       // A-Z / a-z
    boost::spirit::qi::rule<Iterator, char()> bit;    // "0" / "1"
    boost::spirit::qi::rule<Iterator, char()> char_;  // %x01-7F
    boost::spirit::qi::rule<Iterator> cr;             // carriage return
    boost::spirit::qi::rule<Iterator> crlf;           // Internet standard newline
    boost::spirit::qi::ascii::cntrl_type ctl;         // controls
    boost::spirit::qi::ascii::digit_type digit;       // 0-9
    boost::spirit::qi::rule<Iterator, char()> dquote; // " (Double Quote)
    boost::spirit::qi::xdigit_type hexdig;            // DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    boost::spirit::qi::rule<Iterator, char()> htab;   // horizontal tab
    boost::spirit::qi::rule<Iterator> lf;             // linefeed
    boost::spirit::qi::rule<Iterator> lwsp;           // *(WSP / CRLF WSP)
    boost::spirit::qi::byte_type octet;              // 8 bits of data
    boost::spirit::qi::rule<Iterator, char()> sp;
    boost::spirit::qi::rule<Iterator, char()> vchar;  // visible (printing) characters
    boost::spirit::qi::rule<Iterator, char()> wsp;    // white space
};

}

#endif

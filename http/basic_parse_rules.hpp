//
// basic_parse_rules.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_BASIC_PARSE_RULES_HPP
#define HTTP_BASIC_PARSE_RULES_HPP

#include <boost/spirit/home/qi/nonterminal/grammar.hpp>
#include <boost/spirit/home/qi/directive/lexeme.hpp>
#include <boost/spirit/home/qi/binary.hpp>
#include <boost/spirit/home/qi/char.hpp>
#include <boost/spirit/home/qi/operator.hpp>

#include <boost/variant.hpp>

#include <string>
#include <vector>

namespace http {

// The basic parse rules defined by RFC 2616 Section 2.2
template <typename Iter>
struct basic_parse_rules
{
	typedef Iter iterator;

	struct skipper_type : boost::spirit::qi::grammar<iterator>
	{
		skipper_type() : skipper_type::base_type(lws)
		{
			using namespace boost::spirit;

			lws = -(ascii::char_('\r') >> ascii::char_('\n')) >> +(ascii::char_(' ') | ascii::char_('\t'));
		}

		boost::spirit::qi::rule<iterator> lws;
	};

	skipper_type skipper;

	static const basic_parse_rules<iterator>& get()
	{
		static basic_parse_rules<iterator> rules;
		return rules;
	}

	boost::spirit::qi::byte__type octet;        // = <any 8-bit sequence of data>
	boost::spirit::ascii::char_type char_;     // = <any US-ASCII character (octets 0 - 127)>
	boost::spirit::ascii::upper_type upalpha;   // = <any US-ASCII uppercase letter "A".."Z">
	boost::spirit::ascii::lower_type loalpha;   // = <any US-ASCII lowercase letter "a".."z">
	boost::spirit::ascii::alpha_type alpha;     // = UPALPHA | LOALPHA
	boost::spirit::ascii::digit_type digit;     // = <any US-ASCII digit "0".."9">
	boost::spirit::ascii::cntrl_type ctl;       // = <any US-ASCII control character
	                                            //    (octets 0 - 31) and DEL (127)>
	boost::spirit::qi::rule<iterator, char()> cr;        // = <US-ASCII CR, carriage return (13)>
	boost::spirit::qi::rule<iterator, char()> lf;        // = <US-ASCII LF, linefeed (10)>
	boost::spirit::qi::rule<iterator, char()> sp;        // = <US-ASCII SP, space (32)>
	boost::spirit::qi::rule<iterator, char()> ht;        // = <US-ASCII HT, horizontal-tab (9)>
	boost::spirit::qi::rule<iterator, char()> dq;        // = <US-ASCII double-quote mark (34)>

	boost::spirit::qi::rule<iterator> crlf;
	boost::spirit::qi::rule<iterator> lws;
	boost::spirit::qi::rule<iterator, char()> text;
	boost::spirit::qi::rule<iterator, char()> hex;

	boost::spirit::qi::rule<iterator, std::string()> token;
	boost::spirit::qi::rule<iterator> separators;

	boost::spirit::qi::rule<iterator, std::vector<
		boost::make_recursive_variant<
			std::string, std::vector<boost::recursive_variant_>
			>::type>()
		> comment;
	boost::spirit::qi::rule<iterator, std::string()> ctext_quoted_pair;
	boost::spirit::qi::rule<iterator, char()> ctext;

	boost::spirit::qi::rule<iterator, std::string()> quoted_string;
	boost::spirit::qi::rule<iterator, char()> qdtext;

	boost::spirit::qi::rule<iterator, char()> quoted_pair;

private:
	basic_parse_rules()
	{
		using namespace boost::spirit;

		cr = ascii::char_('\r');
		lf = ascii::char_('\n');
		sp = ascii::char_(' ');
		ht = ascii::char_('\t');
		dq = ascii::char_('"');

		crlf = cr >> lf;
		lws = -crlf >> +(sp | ht);
		text = octet - ctl;
		hex = digit | ascii::char_("A-Fa-f");

		token = +(char_ - ctl - separators);
		separators = ascii::char_("()<>@,;:\\\"/[]?={}") | sp | ht;

		comment = '(' >> *(ctext_quoted_pair | comment) >> ')';
		// Use a separate rule so we can adapt the std::vector<char> into an std::string
		// Qi can't adapt variant<std::vector<char>, B> to variant<std::string, B>
		ctext_quoted_pair = *(ctext | quoted_pair);
		ctext = text - '(' - ')';

		quoted_string = '"' >> *(qdtext | quoted_pair) >> '"';
		qdtext = text - '"';

		quoted_pair = '\\' >> char_;
	}
};

}

#endif
//
// allow.hpp
// ~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_HEADERS_ALLOW_HPP
#define HTTP_HEADERS_ALLOW_HPP

#include <http/basic_parse_rules.hpp>

#include <boost/spirit/home/qi/parse.hpp>

#include <boost/spirit/home/karma/generate.hpp>
#include <boost/spirit/home/karma/string.hpp>
#include <boost/spirit/home/karma/operator.hpp>

#include <boost/mpl/pair.hpp>
#include <boost/mpl/string.hpp>

#include <string>
#include <vector>

namespace http {
namespace headers {

class allow_value
{
public:
	template <typename InputIterator>
	void parse(InputIterator begin, InputIterator end)
	{
		const basic_parse_rules<InputIterator>& b = basic_parse_rules<InputIterator>::get();
		boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, allowed_methods_);
	}

	template <typename OutputIterator>
	void serialize(OutputIterator sink) const
	{
		using namespace boost::spirit;
		karma::generate(sink, karma::string % ',', allowed_methods_);
	}

	void clear()
	{
		allowed_methods_.clear();
	}

private:
	std::vector<std::string> allowed_methods_;
};

typedef boost::mpl::pair<boost::mpl::string<'allo', 'w'>, allow_value> allow;

}
}

#endif
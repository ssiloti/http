//
// content_length.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_HEADERS_CONTENT_LENGTH_HPP
#define HTTP_HEADERS_CONTENT_LENGTH_HPP

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/qi/numeric/uint.hpp>
#include <boost/spirit/home/karma/generate.hpp>
#include <boost/spirit/home/karma/numeric/uint.hpp>

#include <boost/fusion/support/pair.hpp>
#include <boost/mpl/string.hpp>

#include <string>
#include <iterator>

namespace http {
namespace headers {

class content_length_value
{
public:
	content_length_value()
		: length_(0)
	{}

	content_length_value(std::size_t length)
		: length_(length)
	{}

	void parse(const std::string& value)
	{
		std::string::const_iterator begin(value.begin());
		boost::spirit::qi::parse(begin, value.end(), boost::spirit::qi::uint_, length_);
	}

	std::vector<std::string> serialize()
	{
		std::string value;
		std::back_insert_iterator<std::string> sink(value);

		boost::spirit::karma::generate(sink, boost::spirit::karma::uint_, length_);

		return std::vector<std::string>(1, value);
	}

	void reset()
	{
		length_ = 0;
	}

	operator std::size_t()
	{
		return length_;
	}

private:
	std::size_t length_;
};

typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'leng', 'th'>, content_length_value> content_length;

}
}

#endif
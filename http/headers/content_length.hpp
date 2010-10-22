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

#include <boost/spirit/include/qi_parse_auto.hpp>
#include <boost/spirit/home/qi/numeric.hpp>
#include <boost/spirit/include/karma_generate_auto.hpp>
#include <boost/spirit/home/karma/numeric.hpp>

#include <boost/fusion/support/pair.hpp>
#include <boost/mpl/string.hpp>

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

	template <typename InputIterator>
	void parse(InputIterator begin, InputIterator end)
	{
		boost::spirit::qi::parse(begin, end, length_);
	}

	template <typename OutputIterator>
	void serialize(OutputIterator sink) const
	{
		boost::spirit::karma::generate(sink, length_);
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
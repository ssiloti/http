//
// parse_known_header.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_DETAIL_PARSE_KNOWN_HEADER_HPP
#define HTTP_DETAIL_PARSE_KNOWN_HEADER_HPP

#include <boost/logic/tribool.hpp>
#include <string>

namespace http {
namespace detail {

template <typename InputIterator>
struct parse_known_header
{
	typedef InputIterator iterator;

	// Add we really need is a bool that default constructs to false.
	// I used boost::tribool beause I'm lazy.
	typedef boost::tribool result_type;

	parse_known_header(iterator begin, iterator end)
		: begin_(begin), end_(end)
	{}

	template <typename Header>
	result_type operator()(Header header) const
	{
		header.second.parse(begin_, end_);
		return true;
	}

	const iterator begin_, end_;
};

}
}

#endif

//
// serialize_known_header.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_DETAIL_SERIALIZE_KNOWN_HEADER_HPP
#define HTTP_DETAIL_SERIALIZE_KNOWN_HEADER_HPP

#include <string>
#include <vector>

namespace http {
namespace detail {

template <typename OutputIterator>
struct serialize_known_header
{
	typedef OutputIterator iterator;
	typedef void result_type;

	serialize_known_header(iterator sink)
		: sink_(sink)
	{}

	template <typename Header>
	result_type operator()(Header header) const
	{
		header.second.serialize(sink_);
	}

	iterator sink_;
};

}
}

#endif

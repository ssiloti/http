//
// reset_known_header.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_DETAIL_RESET_KNOWN_HEADER_HPP
#define HTTP_DETAIL_RESET_KNOWN_HEADER_HPP

namespace http {
namespace detail {

struct reset_known_header
{
	typedef void result_type;

	template <typename Header>
	result_type operator()(Header& header) const
	{
		header.second.reset();
	}
};

}
}

#endif

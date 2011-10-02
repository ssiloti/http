//
// basic_uri.ipp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef URI_BASIC_URI_IPP
#define URI_BASIC_URI_IPP

#include <uri/parsers/generic_uri.hpp>
#include <boost/spirit/home/qi/parse.hpp>

namespace uri {

template <typename String>
basic_uri<String>::basic_uri(string_type const& uri)
{
	typename string_type::const_iterator begin = uri.begin();
	boost::spirit::qi::parse(begin, uri.end(), parsers::generic_uri<typename string_type::const_iterator, string_type>(), *this);
}

template <typename String>
basic_uri<String>::basic_uri(const typename string_type::value_type* uri)
{
	const typename string_type::value_type* end = uri;
	while (*end) ++end;
	boost::spirit::qi::parse(uri, end, parsers::generic_uri<const typename string_type::value_type*, string_type>(), *this);
}

}

#endif
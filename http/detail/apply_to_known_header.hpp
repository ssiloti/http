//
// apply_to_known_header.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2001-2006 Joel de Guzman
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_DETAIL_APPLY_TO_KNOWN_HEADER_HPP
#define HTTP_DETAIL_APPLY_TO_KNOWN_HEADER_HPP

#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/iterator/equal_to.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/string.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace http {
namespace detail {

template <typename First, typename Last, typename F>
inline typename F::result_type
apply_to_known_header_linear(First const&,
                             Last const&,
                             std::string,
                             F const&,
                             boost::mpl::true_)
{
	return typename F::result_type();
}

template <typename First, typename Last, typename F>
inline typename F::result_type
apply_to_known_header_linear(First const& first,
                             Last const& last,
                             std::string name,
                             F const& f,
                             boost::mpl::false_)
{
	using namespace boost;

	typedef typename fusion::result_of::deref<First>::type item_type;
	if (name == mpl::c_str<typename remove_reference<item_type>::type::first_type>::value)
		return f(*first);
	else
		return apply_to_known_header_linear(
			  fusion::next(first), last, name, f
			, fusion::result_of::equal_to<typename fusion::result_of::next<First>::type, Last>()
			);
}


template <typename Sequence, typename F>
inline typename F::result_type
apply_to_known_header(Sequence& seq, std::string name, F const& f)
{
	using namespace boost;

	return apply_to_known_header_linear(
		  fusion::begin(seq), fusion::end(seq), name, f
		, fusion::result_of::equal_to<typename fusion::result_of::begin<Sequence>::type
		, typename fusion::result_of::end<Sequence>::type>()
		);
}

}
}

#endif

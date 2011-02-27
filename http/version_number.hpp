//
// version_number.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_VERSION_NUMBER_HPP
#define HTTP_VERSION_NUMBER_HPP

#include <boost/fusion/include/adapt_struct.hpp>

namespace http {

struct version_number
{
    unsigned int major, minor;
};

}

BOOST_FUSION_ADAPT_STRUCT(
    http::version_number,
    (unsigned int, major)
    (unsigned int, minor)
)

#endif

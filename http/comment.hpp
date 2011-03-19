//
// comment.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_COMMENT_HPP
#define HTTP_COMMENT_HPP

#include <boost/variant.hpp>

#include <vector>
#include <string>

namespace http {

typedef std::vector<
        boost::make_recursive_variant<
            std::string, std::vector<boost::recursive_variant_>
        >::type
> comment;

}

#endif

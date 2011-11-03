//
// entity_tag.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ENTITY_TAG_HPP
#define HTTP_ENTITY_TAG_HPP

#include <string>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

namespace http {

struct entity_tag
{
    bool weak;
    std::string tag;
};

}

BOOST_FUSION_ADAPT_STRUCT(
    http::entity_tag,
    (bool, weak)
    (std::string, tag)
)

#endif

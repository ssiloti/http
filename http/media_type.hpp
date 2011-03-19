//
// media_type.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_MEDIA_TYPE_HPP
#define HTTP_MEDIA_TYPE_HPP

#include <boost/fusion/include/adapt_struct.hpp>

#include <string>
#include <map>

namespace http {

typedef std::map<std::string, std::string> parameters_t;

struct media_type
{
    void clear()
    {
        type.clear();
        subtype.clear();
        parameters.clear();
    }

    std::string type, subtype;
    parameters_t parameters;
};

}

BOOST_FUSION_ADAPT_STRUCT(
    http::media_type,
    (std::string, type)
    (std::string, subtype)
    (http::parameters_t, parameters)
)

#endif
